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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>

#include <QModelIndex>

#include "api_key.h"
#include <spotify/api.h>

#if 1
#include "audio.h"
#endif

class QLabel;
class QBuffer;
class QListView;
class QProgressBar;

class KAction;
class KComboBox;
class KLineEdit;
class KStatusNotifierItem;

class MainWidget;
class PlaylistModel;

class MainWindow
    : public KXmlGuiWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    sp_session *session() const;

    static MainWindow *self();
    MainWidget *mainWidget() const;

    void spotifyLoggedIn();
    void spotifyLoggedOut();

    void showTemporaryMessage(const QString &message);
    void showRequest(const QString &request);

    audio_fifo_t *audioFifo();

public Q_SLOTS:
    void restoreStatusBarSlot();

protected:
    virtual bool event(QEvent *event);

private Q_SLOTS:
    void loginSlot();
    void logoutSlot();
    void performSearch();
    void playListChanged(const QModelIndex &index);
    void trackRequested(const QModelIndex &index);

private:
    QWidget *createSearchWidget();
    void setupActions();
    void clearAllWidgets();
    void fillPlaylistModel();

private:
    sp_session_config     m_config;
    sp_session           *m_session;
    sp_playlistcontainer *m_pc;
    sp_search            *m_search;

    KAction              *m_login;
    KAction              *m_logout;
    QLabel               *m_statusLabel;
    QProgressBar         *m_progress;
    KStatusNotifierItem  *m_notifierItem;
    bool                  m_loggedIn;
    audio_fifo_t          m_audioFifo;
    static MainWindow    *s_self;

    KComboBox            *m_searchCategory;
    KLineEdit            *m_searchField;

    MainWidget           *m_mainWidget;
    PlaylistModel        *m_playlistModel;
    QListView            *m_playlistView;
};
 
#endif
