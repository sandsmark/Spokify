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

#ifndef SCROBBLER_H
#define SCROBBLER_H

#include <QObject>
#include <QString>
#include <Audioscrobbler.h>
#include <Track.h>

namespace lastfm {
    class Audioscrobbler;
    class MutableTrack;
}

namespace KWallet {
    class Wallet;
}

class Scrobbler : public QObject
{
    Q_OBJECT

public:
    Scrobbler(QWidget *parent);

public slots:
    void setTrack(const QString &artist, const QString &title, const uint duration);
    void scrobble();

private:
    lastfm::Audioscrobbler *m_as;
    lastfm::MutableTrack m_track;
    bool m_isSetup;
    KWallet::Wallet *m_wallet;
};

#endif//SCROBBLER_H
