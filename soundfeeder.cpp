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

#include "soundfeeder.h"
#include "mainwindow.h"
#include <QDebug>

SoundFeeder::SoundFeeder(QObject *parent)
    : QThread(parent)
{
}

SoundFeeder::~SoundFeeder()
{
}

void SoundFeeder::run()
{
    Q_FOREVER {
        QMutex &m = MainWindow::self()->dataMutex();
        m.lock();
        while (!MainWindow::self()->hasChunk() && !MainWindow::self()->isExiting()) {
            MainWindow::self()->pcmWaitCondition().wait(&m);
        }
        if (MainWindow::self()->isExiting()) {
            m.unlock();
            break;
        }
        Chunk c = MainWindow::self()->nextChunk();
        m.unlock();
        if (c.m_dataFrames == -1) {
            emit pcmWritten(c);
            continue;
        }
        QMutex &m2 = MainWindow::self()->pcmMutex();
        m2.lock();
        while (!MainWindow::self()->isPlaying()) {
            MainWindow::self()->playCondition().wait(&m2);
        }
        const int written = snd_pcm_writei(MainWindow::self()->pcmHandle(), c.m_data, c.m_dataFrames);
        if (written < 0) {
            snd_pcm_recover(MainWindow::self()->pcmHandle(), written, 1);
        }
        m2.unlock();
        free(c.m_data);
        if (MainWindow::self()->isPlaying()) {
            emit pcmWritten(c);
        }
        usleep(10000);
    }
}
