/*
 * This file is part of Spokify.
 * Copyright (C) 2010 Martin Sandsmark <martin.sandsmark@kde.org>
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

#include "scrobblingsettingsdialog.h"

// KDE includes
#include <KLocale>
#include <KWallet/Wallet>

// Qt includes
#include <QGridLayout>
#include <QDebug>

// Other includes
#include <ws.h>
#include <misc.h>
#include <XmlQuery.h>

ScrobblingSettingsDialog::ScrobblingSettingsDialog(QWidget* parent):
    QDialog(parent),
    m_wallet(0),
    m_isValid(false)
{
    QGridLayout *layout = new QGridLayout;
    setLayout(layout);

    m_testButton.setText(i18n("Test credentials"));
    m_saveButton.setText(i18n("Save credentials"));
    
    m_password.setEchoMode(QLineEdit::Password);

    layout->addWidget(new QLabel(i18n("Please enter a valid username and password:")), 0, 0, 1, 2);
    layout->addWidget(new QLabel(i18n("Username:")), 1, 0);
    layout->addWidget(&m_username, 1, 1);
    layout->addWidget(new QLabel(i18n("Password:")), 2, 0);
    layout->addWidget(&m_password, 2, 1);
    layout->addWidget(&m_label, 3, 0, 1, 2);
    layout->addWidget(&m_testButton, 4, 0);
    layout->addWidget(&m_saveButton, 4, 1);

    connect(&m_testButton, SIGNAL(clicked()), this, SLOT(testCredentials()));
    connect(&m_saveButton, SIGNAL(clicked()), this, SLOT(saveCredentials()));

    m_wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(), winId());
    if (m_wallet) {
        if (!m_wallet->hasFolder("spokify")) {
            m_wallet->createFolder("spokify");
        }
        m_wallet->setFolder("spokify");
    }
}

void ScrobblingSettingsDialog::showEvent(QShowEvent* event)
{
    if (m_wallet) {
        QMap<QString, QString> authInfo;
        m_wallet->readMap("last.fm", authInfo);
        m_username.setText(authInfo["username"]);
        m_password.setText(authInfo["password"]);
        m_sessionKey = authInfo["key"];
    }

    QDialog::showEvent(event);
}


ScrobblingSettingsDialog::~ScrobblingSettingsDialog()
{
    if (m_wallet)
        delete m_wallet;
}

void ScrobblingSettingsDialog::saveCredentials()
{
    if (!m_isValid) {
        m_label.setText(i18n("Please enter valid credentials and test them before saving!"));
        return;
    }

    if (m_wallet) {
        QMap<QString, QString> authInfo;
        authInfo["username"] = m_username.text();
        authInfo["password"] = m_password.text();
        authInfo["key"] = m_sessionKey;
        m_wallet->writeMap("last.fm", authInfo);
        m_wallet->sync();
    }

    QDialog::accept();
}

void ScrobblingSettingsDialog::testCredentials()
{
    lastfm::ws::Username = m_username.text();
    lastfm::ws::ApiKey = "3e6ecbd7284883089e8f2b5b53b0aecd";
    lastfm::ws::SharedSecret = "2cab3957b1f70d485e9815ac1ac94096";

    QMap<QString, QString> params;
    params["method"] = "auth.getMobileSession";
    params["username"] = lastfm::ws::Username;
    params["authToken"] = lastfm::md5((lastfm::ws::Username + lastfm::md5(m_password.text().toUtf8())).toUtf8());

    m_label.setText("Sending authentication request to last.fm...");
    m_networkReply = lastfm::ws::post(params);
    connect(m_networkReply, SIGNAL(finished()), this, SLOT(gotNetworkReply()));
}

void ScrobblingSettingsDialog::gotNetworkReply()
{
    m_label.setText("Received reply from last.fm!");
    try {
        lastfm::XmlQuery reply;
        reply.parse(m_networkReply->readAll());
        lastfm::ws::Username = reply["session"]["name"].text();
        m_sessionKey = reply["session"]["key"].text();
        m_label.setText("Authentication successfull!");
        m_isValid = true;
    } catch (std::runtime_error &e) {
        qWarning() << "Caught a fucking exception from the bloody liblastfm POS: " << e.what();
        m_label.setText("Authentication failed!");
        m_isValid = false;
    }
}
