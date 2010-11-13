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

#ifndef SCROBBLINGSETTINGSDIALOG_H
#define SCROBBLINGSETTINGSDIALOG_H

#include <QDialog> // KDialog has some layouting bugs

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class QNetworkReply;

namespace KWallet {
    class Wallet;
}

class ScrobblingSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ScrobblingSettingsDialog(QWidget* parent = 0);
    virtual ~ScrobblingSettingsDialog();

private slots:
    void testCredentials();
    void saveCredentials();

    void gotNetworkReply();

protected:
    virtual void showEvent(QShowEvent *event);

private:
    QLineEdit m_username;
    QLineEdit m_password;
    QPushButton m_testButton;
    QPushButton m_saveButton;
    KWallet::Wallet *m_wallet;
    bool m_isValid;
    QLabel m_label;
    QNetworkReply *m_networkReply;
    QString m_sessionKey;
};

#endif//SCROBBLINGSETTINGSDIALOG_H