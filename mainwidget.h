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

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "chunk.h"

#include <QtCore/QModelIndex>

#include <QtGui/QWidget>
#include <QtGui/QItemSelection>

class TrackView;
class TrackModel;

class QLabel;
class QSlider;
class QTabWidget;
class QSortFilterProxyModel;

class KLineEdit;

class Slider;
class PlayPauseButton;

class MainWidget
    : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = 0);
    virtual ~MainWidget();

    void loggedIn();
    void loggedOut();

    void clearFilter();

    TrackModel *newTrackModel();
    TrackModel *trackModel() const;
    TrackModel *trackPlayingModel() const;
    TrackView *trackView() const;

    void setTotalTrackTime(int totalTrackTime);
    void advanceCurrentTrackTime(const Chunk &c);
    void advanceCurrentCacheTrackTime(const Chunk &c);

Q_SIGNALS:
    void play(const QModelIndex &index);
    void resume();
    void seekPosition(int position);
    void currentTrackFinished();

private Q_SLOTS:
    void playSlot();
    void pauseSlot();
    void sliderReleasedSlot();
    void trackRequested(const QModelIndex &index);
    void selectionChangedSlot(const QItemSelection &selection);

private:
    enum State {
        Stopped = 0,
        Playing,
        Paused
    };

    State                  m_state;
    KLineEdit             *m_filter;
    TrackView             *m_trackView;
    TrackModel            *m_trackModel;
    TrackModel            *m_trackPlayingModel;
    QSortFilterProxyModel *m_proxyModel;
    PlayPauseButton       *m_playPauseButton;
    Slider                *m_slider;
    QLabel                *m_currTotalTime;
};

#endif
