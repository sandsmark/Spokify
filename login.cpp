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

#include "login.h"
#include "mainwindow.h"

#include <QtGui/QFormLayout>

#include <KLineEdit>
#include <KLocale>

Login::Login(MainWindow *mainWindow)
    : KDialog(mainWindow)
    , m_username(new KLineEdit(this))
    , m_password(new KLineEdit(this))
    , m_mainWindow(mainWindow)
{
    setWindowTitle(i18n("Login"));
    setButtons(KDialog::Ok | KDialog::Cancel);

    m_password->setEchoMode(KLineEdit::Password);

    QWidget *main = new QWidget(this);
    QFormLayout *layout = new QFormLayout;
    layout->addRow(i18n("Username"), m_username);
    layout->addRow(i18n("Password"), m_password);
    main->setLayout(layout);

    connect(this, SIGNAL(okClicked()), SLOT(loginSlot()));

    setMainWidget(main);
}

Login::~Login()
{
}

void Login::loginSlot()
{
    sp_session_login(m_mainWindow->session(), m_username->text().toLatin1(),
                     m_password->text().toLatin1());
}
