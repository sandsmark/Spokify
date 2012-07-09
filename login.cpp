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

#include <QtGui/QLabel>
#include <QtGui/QCheckBox>
#include <QtGui/QFormLayout>

#include <KLocale>
#include <KLineEdit>
#include <kwallet.h>

Login::Login(MainWindow *mainWindow)
    : KDialog(mainWindow)
    , m_username(new KLineEdit(this))
    , m_password(new KLineEdit(this))
    , m_remember(new QCheckBox(i18n("Remember me"), this))
    , m_mainWindow(mainWindow)
{
    setWindowTitle(i18n("Login"));
    setButtons(KDialog::Ok | KDialog::Cancel);

    m_password->setEchoMode(KLineEdit::Password);
    m_remember->setCheckState(Qt::Checked);

    QWidget *main = new QWidget(this);
    QFormLayout *layout = new QFormLayout;
    layout->addRow(i18n("Username"), m_username);
    layout->addRow(i18n("Password"), m_password);
    layout->addWidget(m_remember);
    QLabel *note = new QLabel(i18n("Note that for logging in you need a Premium Account"), main);
    note->setWordWrap(true);
    layout->addWidget(note);
    main->setLayout(layout);

    connect(this, SIGNAL(okClicked()), SLOT(loginSlot()));

    setMainWidget(main);

    m_wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(), winId());
    if (m_wallet) {
        if (!m_wallet->hasFolder("spokify")) {
            m_wallet->createFolder("spokify");
        }
        m_wallet->setFolder("spokify");
    }
}

Login::~Login()
{
}

void Login::accept()
{
    if (m_wallet && m_remember->checkState() == Qt::Checked) {
        QMap<QString, QString> authInfo;
        authInfo["username"] = m_username->text();
        authInfo["password"] = m_password->text();
        m_wallet->writeMap("spokify", authInfo);
        m_wallet->sync();
    }

    KDialog::accept();
}

void Login::showEvent(QShowEvent *event)
{
    if (m_wallet) {
        QMap<QString, QString> authInfo;
        m_wallet->readMap("spokify", authInfo);
        m_username->setText(authInfo["username"]);
        m_password->setText(authInfo["password"]);
    }

    KDialog::showEvent(event);
}

void Login::loginSlot()
{
    //BEGIN: Spotify login
    sp_session_login(m_mainWindow->session(), m_username->text().toLatin1(),
                     m_password->text().toLatin1(), true
#if SPOTIFY_API_VERSION >= 11
                     , NULL
#endif
                     );
    //END: Spotify login
}
