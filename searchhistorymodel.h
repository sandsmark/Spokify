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

#ifndef SEARCHHISTORYMODEL_H
#define SEARCHHISTORYMODEL_H

#include <QtCore/QList>
#include <QtCore/QAbstractListModel>

struct sp_search;

class SearchHistoryModel
    : public QAbstractListModel
{
public:
    enum OwnRoles {
        SpotifyNativeSearchRole = Qt::UserRole
    };

    SearchHistoryModel(QObject *parent = 0);
    virtual ~SearchHistoryModel();

    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole);
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

private:
    struct Entry {
        QString    m_title;
        sp_search *m_search;
    };
    QList<Entry> m_searchHistory;
};

Q_DECLARE_METATYPE(sp_search*)

#endif
