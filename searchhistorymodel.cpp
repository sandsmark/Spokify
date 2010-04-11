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

#include "searchhistorymodel.h"

SearchHistoryModel::SearchHistoryModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

SearchHistoryModel::~SearchHistoryModel()
{
}

bool SearchHistoryModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row > m_searchHistory.count() || count <= 0) {
        return false;
    }
    beginInsertRows(parent, row, row + count - 1);
    for (int i = row; i < row + count; ++i) {
        m_searchHistory << Entry();
    }
    endInsertRows();
    return true;
}

bool SearchHistoryModel::removeRows(int row, int count, const QModelIndex & parent)
{
    if (row < 0 || row >= m_searchHistory.count() || count <= 0) {
        return false;
    }
    beginRemoveRows(parent, row, row + count - 1);
    for (int i = row; i < row + count; ++i) {
        m_searchHistory.removeAt(row);
    }
    endRemoveRows();
    return true;
}

bool SearchHistoryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }
    switch (role) {
        case Qt::DisplayRole:
            m_searchHistory[index.row()].m_title = value.toString();
            break;
        case SpotifyNativeSearchRole:
            m_searchHistory[index.row()].m_search = value.value<sp_search*>();
            break;
        default:
            return false;
    }
    emit dataChanged(index, index);
    return true;
}

QVariant SearchHistoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    switch (role) {
        case Qt::DisplayRole:
            return m_searchHistory[index.row()].m_title;
        case SpotifyNativeSearchRole:
            return QVariant::fromValue<sp_search*>(m_searchHistory[index.row()].m_search);
        default:
            break;
    }
    return QVariant();
}

int SearchHistoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_searchHistory.count();
}
