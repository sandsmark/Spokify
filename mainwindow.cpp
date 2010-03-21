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

#include "mainwindow.h"
#include "login.h"

#include <KDebug>
#include <KAction>
#include <KLocale>
#include <KApplication>
#include <KStandardAction>
#include <KActionCollection>

namespace Spotify {

    static void loggedIn(sp_session *session, sp_error error)
    {
        if (SP_ERROR_OK != error) {
            kDebug() << "Failed to log in to Spotify:" << sp_error_message(error);
            return;
        }

        sp_user *me = sp_session_user(session);
        const char *my_name = (sp_user_is_loaded(me) ?
                               sp_user_display_name(me) :
                               sp_user_canonical_name(me));

        kDebug() << "Logged in to Spotify as user" << my_name;
    }

    static sp_session_callbacks spotifyCallbacks = {
        &Spotify::loggedIn,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    };

}

MainWindow::MainWindow(QWidget *parent)
  : KXmlGuiWindow(parent)
{
    setCentralWidget(new QWidget(this));
    setupActions();

    m_config.api_version = SPOTIFY_API_VERSION;
    m_config.cache_location = "tmp";
    m_config.settings_location = "tmp";
    m_config.application_key = g_appkey;
    m_config.application_key_size = g_appkey_size;
    m_config.user_agent = "spokify";
    m_config.callbacks = &Spotify::spotifyCallbacks;

    sp_session_init(&m_config, &m_session);

    startTimer(500);
}

sp_session *MainWindow::session() const
{
    return m_session;
}

bool MainWindow::event(QEvent *event)
{
    switch (event->type()) {
        case QEvent::Timer: {
            int timeout = -1;
            sp_session_process_events(m_session, &timeout);
            event->accept();
            return true;
        }
        default:
            break;
    }
    return KXmlGuiWindow::event(event);
}

void MainWindow::loginSlot()
{
    Login *login = new Login(this);
    login->exec();
}

void MainWindow::setupActions()
{
    KAction *login = new KAction(this);
    login->setText(i18n("&Login"));
    login->setIcon(KIcon("view-media-artist"));
    login->setShortcut(Qt::CTRL + Qt::Key_L);
    actionCollection()->addAction("login", login);
    connect(login, SIGNAL(triggered(bool)), this, SLOT(loginSlot()));

    KStandardAction::quit(kapp, SLOT(quit()), actionCollection());

    setupGUI(Default, "spokifyui.rc");
}
