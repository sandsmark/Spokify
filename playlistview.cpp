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

#include "playlistview.h"

#include <QtGui/QMenu>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>

#include <KLocale>

PlaylistView::PlaylistView(QWidget *parent)
    : QListView(parent)
    , m_contextMenu(new QMenu(this))
{
    m_contextMenu->addAction(i18n("Create new Playlist"));
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(i18n("Rename Playlist"));
    m_contextMenu->addAction(i18n("Delete Playlist"));
}

PlaylistView::~PlaylistView()
{
}

QSize PlaylistView::sizeHint() const
{
    return QSize(600,500);
}

void PlaylistView::dragEnterEvent(QDragEnterEvent *event)
{
    //TODO: check permissions...
    event->accept();
}

void PlaylistView::dragLeaveEvent(QDragLeaveEvent *event)
{
    //TODO: check permissions...
    event->accept();
}

void PlaylistView::dragMoveEvent(QDragMoveEvent *event)
{
    //TODO: check permissions...
    event->accept();
}

void PlaylistView::contextMenuEvent(QContextMenuEvent *event)
{
    m_contextMenu->popup(QPoint(event->globalX(), event->globalY()));
}
