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

#include <QtGui/QListView>
#include <QtGui/QTabWidget>
#include <QtGui/QBoxLayout>

#include <KLocale>

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
    m_playlistView = new QListView(this);
    m_trackModel = new TrackModel(this);
    m_playlistView->setModel(m_trackModel);
    return m_playlistView;
}
