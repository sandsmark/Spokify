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

TrackModel::TrackModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

TrackModel::~TrackModel()
{
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
    if (!index.isValid() || role != Qt::DisplayRole) {
        return false;
    }
    m_tracks[index.row()].m_title = value.toString();
    emit dataChanged(index, index);
    return true;
}

QVariant TrackModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    return m_tracks[index.row()].m_title;
}

int TrackModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_tracks.count();
}
