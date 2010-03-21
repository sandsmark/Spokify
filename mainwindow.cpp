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

#include <QtGui/QLabel>
#include <QtGui/QMovie>

#include <KDebug>
#include <KAction>
#include <KLocale>
#include <KStatusBar>
#include <KApplication>
#include <KStandardDirs>
#include <KStandardAction>
#include <KActionCollection>

MainWindow *MainWindow::s_self = 0;

//BEGIN: SpotifySession - application bridge
namespace SpotifySession {

    static void loggedIn(sp_session *session, sp_error error)
    {
        MainWindow::self()->spotifyLoggedIn();
    }

    static void loggedOut(sp_session *session)
    {
        MainWindow::self()->spotifyLoggedOut();
    }

    static void metadataUpdated(sp_session *session)
    {
    }

    static void connectionError(sp_session *session, sp_error error)
    {
    }

    static void messageToUser(sp_session *session, const char *message)
    {
    }

    static void notifyMainThread(sp_session*)
    {
        // Nothing to do. We have our own polling system.
    }

    static int musicDelivery(sp_session *session, const sp_audioformat *format, const void *frames,
                             int numFrames)
    {
        return 0;
    }

    static void playTokenLost(sp_session *session)
    {
    }

    static void logMessage(sp_session *session, const char *data)
    {
    }

    static void endOfTrack(sp_session *session)
    {
    }

    static sp_session_callbacks spotifyCallbacks = {
        &SpotifySession::loggedIn,
        &SpotifySession::loggedOut,
        &SpotifySession::metadataUpdated,
        &SpotifySession::connectionError,
        &SpotifySession::messageToUser,
        &SpotifySession::notifyMainThread,
        &SpotifySession::musicDelivery,
        &SpotifySession::playTokenLost,
        &SpotifySession::logMessage,
        &SpotifySession::endOfTrack
    };

}
//END: SpotifySession - application bridge

MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
    , m_statusLabel(new QLabel(i18n("Ready"), this))
    , m_progress(new QLabel(this))
{
    s_self = this;

    setCentralWidget(new QWidget(this));
    setupActions();

    //BEGIN: Spotify session init
    {
        m_config.api_version = SPOTIFY_API_VERSION;
        m_config.cache_location = "tmp";
        m_config.settings_location = "tmp";
        m_config.application_key = g_appkey;
        m_config.application_key_size = g_appkey_size;
        m_config.user_agent = "spokify";
        m_config.callbacks = &SpotifySession::spotifyCallbacks;

        sp_session_init(&m_config, &m_session);
    }
    //END: Spotify session init

    QMovie *movie = new QMovie(KStandardDirs::locate("appdata", "images/loading.gif"));
    m_progress->setMovie(movie);
    m_progress->setVisible(false);

    startTimer(500);
    statusBar()->insertWidget(0, m_statusLabel);
    statusBar()->insertWidget(1, new QWidget(this), 1);
    statusBar()->insertWidget(2, m_progress);
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
    m_progress->movie()->stop();
    m_progress->setVisible(false);
    m_statusLabel->setText(message);
    QTimer::singleShot(2000, this, SLOT(restoreStatusBarSlot()));
}

void MainWindow::showRequest(const QString &request)
{
    m_progress->movie()->start();
    m_progress->setVisible(true);
    m_statusLabel->setText(request);
}

bool MainWindow::event(QEvent *event)
{
    //BEGIN: Spotify event processing
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
    //END: Spotify event processing
    return KXmlGuiWindow::event(event);
}

void MainWindow::loginSlot()
{
    Login *login = new Login(this);
    if (login->exec() == KDialog::Accepted) {
        m_login->setEnabled(false);
        showRequest(i18n("Logging in..."));
    }
}

void MainWindow::logoutSlot()
{
    //BEGIN: Spotify logout
    sp_session_logout(m_session);
    //END: Spotify logout
    m_logout->setEnabled(false);
    showRequest(i18n("Logging out..."));
}

void MainWindow::restoreStatusBarSlot()
{
    m_progress->movie()->stop();
    m_progress->setVisible(false);
    m_statusLabel->setText(i18n("Ready"));
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
