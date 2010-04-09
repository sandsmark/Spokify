/*
 * This file is part of Spokify.
 * Copyright (C) 2010 Rafael Fernández López <ereslibre@kde.org>
 *
 * Spokify is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Spokify is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Spokify.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwidget.h"
#include "slider.h"
#include "trackview.h"
#include "trackmodel.h"
#include "playpausebutton.h"
#include "trackviewdelegate.h"

#include <math.h>

#include <QtGui/QLabel>
#include <QtGui/QTabWidget>
#include <QtGui/QBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QSortFilterProxyModel>

#include <KIcon>
#include <KLocale>
#include <KLineEdit>
#include <KPushButton>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , m_state(Stopped)
    , m_currentCollection(0)
    , m_currentPlayingCollection(0)
{
    m_filter = new KLineEdit(this);
    m_filter->setClickMessage(i18n("Filter by title, artist or album"));
    m_filter->setClearButtonShown(true);

    m_trackView = new TrackView(this);
    m_trackView->verticalHeader()->hide();
    m_trackView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_trackView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_trackView->setAlternatingRowColors(true);
    m_trackView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_trackView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_trackView->setShowGrid(false);
    m_trackView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_trackView->setItemDelegate(new TrackViewDelegate(m_trackView));
    m_trackView->setSortingEnabled(true);

    m_playPauseButton = new PlayPauseButton(this);

    m_slider = new Slider(this);
    m_currTotalTime = new QLabel(this);

    connect(m_trackView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(trackRequested(QModelIndex)));
    connect(m_playPauseButton, SIGNAL(play()), this, SLOT(playSlot()));
    connect(m_playPauseButton, SIGNAL(pause()), this, SLOT(pauseSlot()));
    connect(m_slider, SIGNAL(sliderReleased()), this, SLOT(sliderReleasedSlot()));
    connect(m_slider, SIGNAL(maximumReached()), this, SIGNAL(currentTrackFinished()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_filter);
    layout->addWidget(m_trackView);
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(m_playPauseButton);
    hLayout->addWidget(m_slider);
    hLayout->addWidget(m_currTotalTime);
    hLayout->setStretchFactor(m_slider, 1);
    layout->addLayout(hLayout);
    setLayout(layout);
}

MainWidget::~MainWidget()
{
}

void MainWidget::loggedIn()
{
    m_filter->setEnabled(true);
    m_trackView->setEnabled(true);
    m_slider->setEnabled(true);
    m_currTotalTime->setEnabled(true);
}

void MainWidget::loggedOut()
{
    m_state = Stopped;
    m_filter->setEnabled(false);
    m_filter->setText(QString());
    m_trackView->setEnabled(false);
    m_trackView->setModel(0);
    m_playPauseButton->setEnabled(false);
    m_playPauseButton->setIsPlaying(false);
    m_slider->setEnabled(false);
    m_slider->setValue(0);
    m_slider->setCacheValue(0);
    m_currTotalTime->setText(i18n("<b>00:00</b><br/><b>00:00</b>"));
    m_currTotalTime->setEnabled(false);
}

void MainWidget::clearFilter()
{
    m_filter->clear();
}

MainWidget::Collection MainWidget::collection(sp_playlist *playlist)
{
    if (m_trackModelPlaylistCache.contains(playlist)) {
        Collection res = m_trackModelPlaylistCache[playlist];
        res.needsToBeFilled = false;
        m_trackModelPlaylistCache[playlist] = res;
        m_trackView->setModel(res.proxyModel);
        m_trackView->sortByColumn(res.proxyModel->sortColumn(), res.proxyModel->sortOrder());

        connect(m_trackView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChangedSlot(QItemSelection)));

        if (m_currentPlayingCollection && *m_currentPlayingCollection == res && res.currentTrack.isValid()) {
            m_trackView->setCurrentIndex(res.currentTrack);
        }

        return res;
    }

    Collection c;
    c.trackModel = new TrackModel(m_trackView);
    c.proxyModel = new QSortFilterProxyModel(m_trackView);
    c.proxyModel->setFilterKeyColumn(-1);
    c.proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    c.proxyModel->setSortRole(TrackModel::SortRole);
    c.proxyModel->setSourceModel(c.trackModel);
    c.needsToBeFilled = true;
    m_trackModelPlaylistCache[playlist] = c;
    m_currentCollection = &m_trackModelPlaylistCache[playlist];

    connect(m_filter, SIGNAL(textChanged(QString)), c.proxyModel, SLOT(setFilterFixedString(QString))); 
    m_trackView->setModel(c.proxyModel);
    m_trackView->sortByColumn(c.proxyModel->sortColumn(), c.proxyModel->sortOrder());

    connect(m_trackView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChangedSlot(QItemSelection)));

    if (m_currentPlayingCollection && *m_currentPlayingCollection == c && c.currentTrack.isValid()) {
        m_trackView->setCurrentIndex(c.currentTrack);
    }

    return c;
}

MainWidget::Collection MainWidget::collection(sp_search *search)
{
    if (m_trackModelSearchCache.contains(search)) {
        Collection res = m_trackModelSearchCache[search];
        res.needsToBeFilled = false;
        m_currentCollection = &m_trackModelSearchCache[search];
        m_trackView->setModel(res.proxyModel);
        m_trackView->sortByColumn(res.proxyModel->sortColumn(), res.proxyModel->sortOrder());

        connect(m_trackView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChangedSlot(QItemSelection)));

        if (m_currentPlayingCollection && *m_currentPlayingCollection == res && res.currentTrack.isValid()) {
            m_trackView->setCurrentIndex(res.currentTrack);
        }

        return res;
    }

    Collection c;
    c.trackModel = new TrackModel(m_trackView);
    c.proxyModel = new QSortFilterProxyModel(m_trackView);
    c.proxyModel->setFilterKeyColumn(-1);
    c.proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    c.proxyModel->setSortRole(TrackModel::SortRole);
    c.proxyModel->setSourceModel(c.trackModel);
    c.needsToBeFilled = true;
    m_trackModelSearchCache[search] = c;
    m_currentCollection = &m_trackModelSearchCache[search];

    connect(m_filter, SIGNAL(textChanged(QString)), c.proxyModel, SLOT(setFilterFixedString(QString))); 
    m_trackView->setModel(c.proxyModel);
    m_trackView->sortByColumn(c.proxyModel->sortColumn(), c.proxyModel->sortOrder());

    connect(m_trackView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChangedSlot(QItemSelection)));

    if (m_currentPlayingCollection && *m_currentPlayingCollection == c && c.currentTrack.isValid()) {
        m_trackView->setCurrentIndex(c.currentTrack);
    }

    return c;
}

MainWidget::Collection *MainWidget::currentPlayingCollection() const
{
    if (m_currentPlayingCollection) {
        return m_currentPlayingCollection;
    }
    return 0;
}

TrackView *MainWidget::trackView() const
{
    return m_trackView;
}

void MainWidget::setState(State state)
{
    m_state = state;
}

MainWidget::State MainWidget::state() const
{
    return m_state;
}

void MainWidget::setTotalTrackTime(int totalTrackTime)
{
    m_slider->setRange(0, totalTrackTime * (quint64) 44100);
    m_slider->setValue(0);
    m_slider->setCacheValue(0);
    m_currTotalTime->setText(i18n("<b>00:00</b><br/><b>%1:%2</b>").arg((totalTrackTime / 1000) / 60, 2, 10, QLatin1Char('0'))
                                                                  .arg((totalTrackTime / 1000) % 60, 2, 10, QLatin1Char('0')));
}

void MainWidget::advanceCurrentTrackTime(const Chunk &chunk)
{
    m_slider->setValue(m_slider->value() + chunk.m_dataFrames * 1000);
    m_currTotalTime->setText(i18n("<b>%1:%2</b><br/><b>%3:%4</b>").arg((quint64) ((m_slider->value() / (chunk.m_rate * 1000))) / 60, 2, 10, QLatin1Char('0'))
                                                                  .arg((quint64) ((m_slider->value() / (chunk.m_rate * 1000))) % 60, 2, 10, QLatin1Char('0'))
                                                                  .arg((quint64) ((m_slider->maximum() / (chunk.m_rate * 1000))) / 60, 2, 10, QLatin1Char('0'))
                                                                  .arg((quint64) ((m_slider->maximum() / (chunk.m_rate * 1000))) % 60, 2, 10, QLatin1Char('0')));
}

void MainWidget::advanceCurrentCacheTrackTime(const Chunk &chunk)
{
    if (chunk.m_dataFrames != -1) {
        m_slider->setCacheValue(m_slider->cacheValue() + chunk.m_dataFrames * 1000);
    } else {
        m_slider->setCacheValue(m_slider->maximum());
    }
}

void MainWidget::playSlot()
{
    if (!m_trackView->currentIndex().isValid()) {
        return;
    }
    if (m_state == Stopped) {
        trackRequested(m_trackView->currentIndex());
    } else {
        m_state = Playing;
        emit resume();
    }
}

void MainWidget::pauseSlot()
{
    m_state = Paused;
}

void MainWidget::sliderReleasedSlot()
{
    emit seekPosition(m_slider->value() / (quint64) 44100);
}

void MainWidget::trackRequested(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    m_state = Playing;
    m_playPauseButton->setIsPlaying(true);
    m_currentPlayingCollection = m_currentCollection;
    m_currentPlayingCollection->currentTrack = index;
    emit play(index);
}

void MainWidget::selectionChangedSlot(const QItemSelection &selection)
{
    m_playPauseButton->setEnabled(!selection.isEmpty());
}
