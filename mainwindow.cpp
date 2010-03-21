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

#include <QtCore/QTimer>

#include <KDebug>
#include <KAction>
#include <KLocale>
#include <KStatusBar>
#include <KApplication>
#include <KStandardAction>
#include <KActionCollection>

MainWindow *MainWindow::s_self = 0;

namespace Spotify {

    static void loggedIn(sp_session *session, sp_error error)
    {
        MainWindow::self()->spotifyLoggedIn();
    }

    static void loggedOut(sp_session *session)
    {
        MainWindow::self()->spotifyLoggedOut();
    }

    static sp_session_callbacks spotifyCallbacks = {
        &Spotify::loggedIn,
        &Spotify::loggedOut,
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
    s_self = this;

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
    statusBar()->showMessage(i18n("Ready"));
}

sp_session *MainWindow::session() const
{
    return m_session;
}

MainWindow *MainWindow::self()
{
    return s_self;
}

void MainWindow::spotifyLoggedIn()
{
    m_login->setVisible(false);
    m_login->setEnabled(true);
    m_logout->setVisible(true);
    showTemporaryMessage(i18n("Logged in"));
}

void MainWindow::spotifyLoggedOut()
{
    m_login->setVisible(true);
    m_logout->setVisible(false);
    m_logout->setEnabled(true);
    showTemporaryMessage(i18n("Logged out"));
}

void MainWindow::showTemporaryMessage(const QString &message)
{
    statusBar()->showMessage(message);
    QTimer::singleShot(2000, this, SLOT(restoreStatusBarSlot()));
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
    if (login->exec() == KDialog::Accepted) {
        m_login->setEnabled(false);
        statusBar()->showMessage(i18n("Logging in..."));
    }
}

void MainWindow::logoutSlot()
{
    sp_session_logout(m_session);
    m_logout->setEnabled(false);
    statusBar()->showMessage(i18n("Logging out..."));
}

void MainWindow::restoreStatusBarSlot()
{
    statusBar()->showMessage(i18n("Ready"));
}

void MainWindow::setupActions()
{
    m_login = new KAction(this);
    m_login->setText(i18n("&Login"));
    m_login->setIcon(KIcon("user-online"));
    m_login->setShortcut(Qt::CTRL + Qt::Key_L);
    actionCollection()->addAction("login", m_login);
    connect(m_login, SIGNAL(triggered(bool)), this, SLOT(loginSlot()));

    m_logout = new KAction(this);
    m_logout->setVisible(false);
    m_logout->setText(i18n("L&ogout"));
    m_logout->setIcon(KIcon("user-offline"));
    m_logout->setShortcut(Qt::CTRL + Qt::Key_O);
    actionCollection()->addAction("logout", m_logout);
    connect(m_logout, SIGNAL(triggered(bool)), this, SLOT(logoutSlot()));    

    KStandardAction::quit(kapp, SLOT(quit()), actionCollection());

    setupGUI(Default, "spokifyui.rc");
}
