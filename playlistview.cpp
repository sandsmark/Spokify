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
#include "mimedata.h"
#include "mainwindow.h"
#include "playlistmodel.h"

#include <libspotify/api.h>

#include <QtGui/QMenu>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QBoxLayout>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>

#include <KLocale>
#include <KDialog>
#include <KMessageBox>

PlaylistView::PlaylistView(QWidget *parent)
    : QListView(parent)
    , m_contextMenu(new QMenu(this))
{
    QAction *const newPlaylist = m_contextMenu->addAction(i18n("Create new Playlist"));
    m_contextMenu->addSeparator();
    QAction *const renamePlaylist = m_contextMenu->addAction(i18n("Rename Playlist"));
    QAction *const deletePlaylist = m_contextMenu->addAction(i18n("Delete Playlist"));

    connect(newPlaylist, SIGNAL(triggered()), this, SLOT(newPlaylistSlot()));
    connect(renamePlaylist, SIGNAL(triggered()), this, SLOT(renamePlaylistSlot()));
    connect(deletePlaylist, SIGNAL(triggered()), this, SLOT(deletePlaylistSlot()));
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
    if (dynamic_cast<const MimeData*>(event->mimeData())) {
        // ### Check if playlist is ours. If not, check if collaborative
        event->accept();
    }
}

void PlaylistView::dragMoveEvent(QDragMoveEvent *event)
{
    if (dynamic_cast<const MimeData*>(event->mimeData())) {
        // ### Check if playlist is ours. If not, check if collaborative
        event->accept();
    }
}

void PlaylistView::dropEvent(QDropEvent *event)
{
    const QModelIndex target = indexAt(event->pos());
    if (!target.isValid()) {
        return;
    }
    const MimeData *mimeData = static_cast<const MimeData*>(event->mimeData());
    const sp_track *trackToAdd[] = { mimeData->track() };
    sp_playlist *targetPlaylist = target.data(PlaylistModel::SpotifyNativePlaylistRole).value<sp_playlist*>();
    sp_playlist_add_tracks(targetPlaylist, trackToAdd, 1, 0, MainWindow::self()->session());
}

void PlaylistView::contextMenuEvent(QContextMenuEvent *event)
{
    m_contextMenu->popup(QPoint(event->globalX(), event->globalY()));
}

void PlaylistView::newPlaylistSlot()
{
    KDialog *dialog = new KDialog(this);
    dialog->setCaption(i18n("New Playlist"));
    dialog->setButtons(KDialog::Ok | KDialog::Cancel);

    QWidget *widget = new QWidget(dialog);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel(i18n("Choose a name for the new playlist:"), widget));
    QLineEdit *playlistName = new QLineEdit(widget);
    layout->addWidget(playlistName);
    widget->setLayout(layout);
    dialog->setMainWidget(widget);

    if (dialog->exec() == KDialog::Accepted) {
        sp_playlistcontainer *const playlistContainer = MainWindow::self()->playlistContainer();
        sp_playlistcontainer_add_new_playlist(playlistContainer, playlistName->text().toUtf8().data());
    }
}

void PlaylistView::renamePlaylistSlot()
{
    sp_playlist *targetPlaylist = currentIndex().data(PlaylistModel::SpotifyNativePlaylistRole).value<sp_playlist*>();

    KDialog *dialog = new KDialog(this);
    dialog->setCaption(i18n("Rename Playlist"));
    dialog->setButtons(KDialog::Ok | KDialog::Cancel);

    QWidget *widget = new QWidget(dialog);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel(i18n("Choose a new name for the playlist \"%1\":", QString::fromUtf8(sp_playlist_name(targetPlaylist))), widget));
    QLineEdit *playlistName = new QLineEdit(widget);
    layout->addWidget(playlistName);
    widget->setLayout(layout);
    dialog->setMainWidget(widget);

    if (dialog->exec() == KDialog::Accepted) {
        sp_playlist_rename(targetPlaylist, playlistName->text().toUtf8().data());
    }
}

void PlaylistView::deletePlaylistSlot()
{
    sp_playlist *targetPlaylist = currentIndex().data(PlaylistModel::SpotifyNativePlaylistRole).value<sp_playlist*>();
    if (KMessageBox::questionYesNo(this, i18n("Are you sure that you want to delete the playlist \"%1\"?", QString::fromUtf8(sp_playlist_name(targetPlaylist))),
                                         i18n("Delete Playlist")) == KMessageBox::Yes) {
        sp_playlistcontainer *const playlistContainer = MainWindow::self()->playlistContainer();
        sp_playlistcontainer_remove_playlist(playlistContainer, currentIndex().row());
    }
}
