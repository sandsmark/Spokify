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

#ifndef SOUND_FEEDER_H
#define SOUND_FEEDER_H

#include <QtCore/QThread>

#include "chunk.h"

class SoundFeeder
    : public QThread
{
    Q_OBJECT

public:
    SoundFeeder(QObject *parent = 0);
    virtual ~SoundFeeder();

Q_SIGNALS:
    void pcmWritten(const Chunk &chunk);

protected:
    virtual void run();
};

#endif
