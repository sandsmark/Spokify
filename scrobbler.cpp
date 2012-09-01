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

#include "scrobbler.h"

#include <KWallet/Wallet>
#include <QWidget>

#include <ws.h>


Scrobbler::Scrobbler (QWidget *parent) :
    QObject(parent),
    m_as(0),
    m_isSetup(false)
{
    m_wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(), parent->winId());
    if (m_wallet) {
        if (!m_wallet->hasFolder("spokify")) {
            m_wallet->createFolder("spokify");
        }
        m_wallet->setFolder("spokify");

        QMap<QString, QString> authInfo;
        m_wallet->readMap("last.fm", authInfo);
        lastfm::ws::Username = authInfo["username"];
        if (lastfm::ws::Username.length()) {
            lastfm::ws::SessionKey = authInfo["key"];
            lastfm::ws::ApiKey = "3e6ecbd7284883089e8f2b5b53b0aecd";
            lastfm::ws::SharedSecret = "2cab3957b1f70d485e9815ac1ac94096";
            m_as = new lastfm::Audioscrobbler("tst");
            m_isSetup = true;
        }
    }
}

void Scrobbler::setTrack(const QString& artist, const QString& title, const uint duration)
{
    if (!m_isSetup)
        return;

    m_track.setArtist(artist);
    m_track.setTitle(title);
    m_track.setDuration(duration);
    m_as->nowPlaying(m_track);
}

void Scrobbler::scrobble()
{
    if (!m_isSetup)
        return;
    
    m_as->cache(m_track);
    m_as->submit();
}
