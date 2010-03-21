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

#include "api_key.h"
#include <spotify/api.h>

class KAction;

class MainWindow
    : public KXmlGuiWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

    sp_session *session() const;

    static MainWindow *self();

    void spotifyLoggedIn();
    void spotifyLoggedOut();

    void showTemporaryMessage(const QString &message);

protected:
    virtual bool event(QEvent *event);

private Q_SLOTS:
    void loginSlot();
    void logoutSlot();
    void restoreStatusBarSlot();

private:
    void setupActions();

private:
    sp_session_config  m_config;
    sp_session        *m_session;
    KAction           *m_login;
    KAction           *m_logout;
    static MainWindow *s_self;
};
 
#endif
