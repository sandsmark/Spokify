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

#ifndef TRACKMODEL_H
#define TRACKMODEL_H

#include <QtCore/QList>
#include <QtCore/QAbstractItemModel>

struct sp_track;

class TrackModel
    : public QAbstractItemModel
{
public:
    enum Columns {
        Title = 0,
        Artist,
        Album,
        Duration,
        Popularity
    };

    enum OwnRoles {
        SpotifyNativeTrackRole = Qt::UserRole,
        SortRole
    };

    TrackModel(QObject *parent = 0);
    virtual ~TrackModel();

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &index) const;
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole);
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    struct Entry {
        QString   m_title;
        QString   m_artist;
        QString   m_album;
        int       m_duration;
        int       m_popularity;
        sp_track *m_track;
    };
    QList<Entry> m_tracks;
};

Q_DECLARE_METATYPE(sp_track*)

#endif
