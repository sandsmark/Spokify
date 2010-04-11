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

#include "trackmodel.h"

#include <KLocale>

TrackModel::TrackModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

TrackModel::~TrackModel()
{
}

QVariant TrackModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical || role != Qt::DisplayRole) {
        return QVariant();
    }
    switch (section) {
        case 0:
            return i18n("Title");
        case 1:
            return i18n("Artist");
        case 2:
            return i18n("Album");
        case 3:
            return i18n("Duration");
        case 4:
            return i18n("Popularity");
        default:
            break;
    }
    return QVariant();
}

QModelIndex TrackModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return createIndex(row, column, 0);
}

QModelIndex TrackModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);

    return QModelIndex();
}

bool TrackModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row > m_tracks.count() || count <= 0) {
        return false;
    }
    beginInsertRows(parent, row, row + count - 1);
    for (int i = row; i < row + count; ++i) {
        m_tracks << Entry();
    }
    endInsertRows();
    return true;
}

bool TrackModel::removeRows(int row, int count, const QModelIndex & parent)
{
    if (row < 0 || row >= m_tracks.count() || count <= 0) {
        return false;
    }
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = row; i < row + count; ++i) {
        m_tracks.removeAt(row);
    }
    endRemoveRows();
    return true;
}

bool TrackModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }
    switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
                case Title:
                    m_tracks[index.row()].m_title = value.toString();
                    break;
                case Artist:
                    m_tracks[index.row()].m_artist = value.toString();
                    break;
                case Album:
                    m_tracks[index.row()].m_album = value.toString();
                    break;
                case Duration:
                    m_tracks[index.row()].m_duration = value.toInt();
                    break;
                case Popularity:
                    m_tracks[index.row()].m_popularity = value.toInt();
                    break;
                default:
                    return false;
            }
            break;
        case SpotifyNativeTrackRole:
            m_tracks[index.row()].m_track = value.value<sp_track*>();
            break;
        default:
            return false;
    }
    emit dataChanged(index, index);
    return true;
}

QVariant TrackModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
                case Title:
                    return m_tracks[index.row()].m_title;
                case Artist:
                    return m_tracks[index.row()].m_artist;
                case Album:
                    return m_tracks[index.row()].m_album;
                case Duration: {
                        const int duration = m_tracks[index.row()].m_duration;
                        return i18n("%1:%2").arg((duration / 1000) / 60, 2, 10, QLatin1Char('0'))
                                            .arg((duration / 1000) % 60, 2, 10, QLatin1Char('0'));
                    }
                case Popularity:
                    return QString();
                default:
                    break;
            }
        case SpotifyNativeTrackRole:
            return QVariant::fromValue<sp_track*>(m_tracks[index.row()].m_track);
        case SortRole:
            switch (index.column()) {
                case Title:
                    return m_tracks[index.row()].m_title;
                case Artist:
                    return m_tracks[index.row()].m_artist;
                case Album:
                    return m_tracks[index.row()].m_album;
                case Duration:
                    return m_tracks[index.row()].m_duration;
                case Popularity:
                    return m_tracks[index.row()].m_popularity;
                default:
                    break;
            }
        default:
            break;
    }
    return QVariant();
}

int TrackModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_tracks.count();
}

int TrackModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 5;
}

Qt::ItemFlags TrackModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsDragEnabled | QAbstractItemModel::flags(index);
}
