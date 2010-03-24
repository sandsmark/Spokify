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
#include "trackmodel.h"
#include "proxymodel.h"
#include "tabledelegate.h"

#include <QtGui/QTableView>
#include <QtGui/QTabWidget>
#include <QtGui/QBoxLayout>
#include <QtGui/QHeaderView>

#include <KLocale>
#include <KLineEdit>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , m_tabWidget(new QTabWidget(this))
{
    m_tabWidget->addTab(playlistTab(), i18n("Current Playlist"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_tabWidget);
    setLayout(layout);
}

MainWidget::~MainWidget()
{
}

TrackModel *MainWidget::trackModel() const
{
    return m_trackModel;
}

QWidget *MainWidget::playlistTab()
{
    QWidget *playlistWidget = new QWidget(this);

    KLineEdit *filter = new KLineEdit(playlistWidget);
    filter->setClickMessage(i18n("Filter by title, artist or album"));
    filter->setClearButtonShown(true);

    m_trackView = new QTableView(playlistWidget);
    m_trackView->verticalHeader()->hide();
    m_trackView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_trackView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_trackView->setAlternatingRowColors(true);
    m_trackView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_trackView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_trackView->setShowGrid(false);
    m_trackView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_trackView->setItemDelegate(new TableDelegate(this));

    m_trackModel = new TrackModel(this);

    ProxyModel *proxyModel = new ProxyModel(this);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setSourceModel(m_trackModel);

    m_trackView->setModel(proxyModel);

    connect(filter, SIGNAL(textChanged(QString)), proxyModel, SLOT(setFilterFixedString(QString))); 
    connect(m_trackView, SIGNAL(activated(QModelIndex)), this, SIGNAL(trackRequest(QModelIndex)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(filter);
    layout->addWidget(m_trackView);
    playlistWidget->setLayout(layout);

    return playlistWidget;
}
