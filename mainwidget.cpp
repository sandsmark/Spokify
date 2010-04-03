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
#include "trackview.h"
#include "trackmodel.h"
#include "playpausebutton.h"
#include "trackviewdelegate.h"

#include <math.h>

#include <QtGui/QLabel>
#include <QtGui/QSlider>
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
{
    KLineEdit *filter = new KLineEdit(this);
    filter->setClickMessage(i18n("Filter by title, artist or album"));
    filter->setClearButtonShown(true);

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

    m_trackModel = new TrackModel(this);

    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setFilterKeyColumn(-1);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setSourceModel(m_trackModel);

    m_trackView->setModel(proxyModel);

    m_playPauseButton = new PlayPauseButton(this);

    m_slider = new QSlider(Qt::Horizontal, this);
    m_currTotalTime = new QLabel(this);

    m_currTotalTime->setText("00:00 - 00:00");

    connect(filter, SIGNAL(textChanged(QString)), proxyModel, SLOT(setFilterFixedString(QString))); 
    connect(m_trackView, SIGNAL(activated(QModelIndex)), this, SLOT(trackRequested(QModelIndex)));
    connect(m_playPauseButton, SIGNAL(play()), this, SIGNAL(resume()));
    connect(m_playPauseButton, SIGNAL(pause()), this, SIGNAL(pause()));
    connect(m_slider, SIGNAL(sliderReleased()), this, SLOT(sliderReleasedSlot()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(filter);
    layout->addWidget(m_trackView);
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(m_playPauseButton);
    hLayout->addWidget(m_slider);
    hLayout->addWidget(m_currTotalTime);
    layout->addLayout(hLayout);
    setLayout(layout);
}

MainWidget::~MainWidget()
{
}

TrackModel *MainWidget::trackModel() const
{
    return m_trackModel;
}

TrackView *MainWidget::trackView() const
{
    return m_trackView;
}

void MainWidget::setTotalTrackTime(int totalTrackTime)
{
    m_slider->setRange(0, totalTrackTime * 44.0);
    m_slider->setValue(0);
    m_currTotalTime->setText(i18n("00:00 - %1:%2").arg((totalTrackTime / 1000) / 60, 2, 10, QLatin1Char('0')).arg((totalTrackTime / 1000) % 60, 2, 10, QLatin1Char('0')));
}

void MainWidget::advanceCurrentTrackTime(int frames)
{
    m_slider->setValue(m_slider->value() + frames);
    m_currTotalTime->setText(i18n("%1:%2 - %3:%4").arg((m_slider->value() / 44000) / 60, 2, 10, QLatin1Char('0')).arg((m_slider->value() / 44000) % 60, 2, 10, QLatin1Char('0')).arg((m_slider->maximum() / 44000) / 60, 2, 10, QLatin1Char('0')).arg((m_slider->maximum() / 44000) % 60, 2, 10, QLatin1Char('0')));
}

void MainWidget::sliderReleasedSlot()
{
    emit seekPosition(m_slider->value() / 44.0);
}

void MainWidget::trackRequested(const QModelIndex &index)
{
    m_playPauseButton->setIsPlaying(true);
    emit play(index);
}
