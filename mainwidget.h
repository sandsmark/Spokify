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
#include <QtCore/QPersistentModelIndex>

#include <QtGui/QWidget>
#include <QtGui/QItemSelection>

#include <libspotify/api.h>

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
    enum State {
        Stopped = 0,
        Playing,
        Paused
    };

    struct Collection {
        Collection()
            : proxyModel(0)
            , trackModel(0)
            , needsToBeFilled(true)
        {
        }

        bool operator==(const Collection &rhs) const
        {
            return proxyModel == rhs.proxyModel && trackModel == rhs.trackModel;
        }

        QSortFilterProxyModel *proxyModel;
        TrackModel            *trackModel;
        QPersistentModelIndex  currentTrack;
        bool                   needsToBeFilled;
    };

    MainWidget(QWidget *parent = 0);
    virtual ~MainWidget();

    void loggedIn();
    void loggedOut();

    void clearFilter();

    Collection &collection(sp_playlist *playlist);
    Collection &collection(sp_search *search);
    Collection *currentPlayingCollection() const;
    TrackView *trackView() const;

    void setState(State state);
    State state() const;

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
    State                            m_state;
    KLineEdit                       *m_filter;
    TrackView                       *m_trackView;
    PlayPauseButton                 *m_playPauseButton;
    Slider                          *m_slider;
    QLabel                          *m_currTotalTime;

    QHash<sp_playlist*, Collection> m_trackModelPlaylistCache;
    QHash<sp_search*, Collection>   m_trackModelSearchCache;
    Collection                     *m_currentCollection;
    Collection                     *m_currentPlayingCollection;
};

#endif
