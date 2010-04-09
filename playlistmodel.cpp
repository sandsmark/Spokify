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

#include "playlistmodel.h"

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

PlaylistModel::~PlaylistModel()
{
}

bool PlaylistModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row > m_playLists.count() || count <= 0) {
        return false;
    }
    beginInsertRows(parent, row, row + count - 1);
    for (int i = row; i < row + count; ++i) {
        m_playLists << Entry();
    }
    endInsertRows();
    return true;
}

bool PlaylistModel::removeRows(int row, int count, const QModelIndex & parent)
{
    if (row < 0 || row >= m_playLists.count() || count <= 0) {
        return false;
    }
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = row; i < row + count; ++i) {
        m_playLists.removeAt(row);
    }
    endRemoveRows();
    return true;
}

bool PlaylistModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }
    switch (role) {
        case Qt::DisplayRole:
            m_playLists[index.row()].m_title = value.toString();
            break;
        case SpotifyNativePlaylistRole:
            m_playLists[index.row()].m_playlist = value.value<sp_playlist*>();
            break;
        default:
            return false;
    }
    emit dataChanged(index, index);
    return true;
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    switch (role) {
        case Qt::DisplayRole:
            return m_playLists[index.row()].m_title;
        case SpotifyNativePlaylistRole:
            return QVariant::fromValue<sp_playlist*>(m_playLists[index.row()].m_playlist);
        default:
            break;
    }
    return QVariant();
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_playLists.count();
}
