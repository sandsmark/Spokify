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

#include "trackviewdelegate.h"

#include <QtGui/QTableView>

TrackViewDelegate::TrackViewDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

TrackViewDelegate::~TrackViewDelegate()
{
}

void TrackViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 opt(*qstyleoption_cast<const QStyleOptionViewItemV4*>(&option));
    const QTableView *const tableView = static_cast<const QTableView*>(opt.widget);
    const QPoint mouse = tableView->viewport()->mapFromGlobal(QCursor::pos());
    const QModelIndex hoveredIndex = tableView->indexAt(mouse);

    if (hoveredIndex.isValid()) {
        if (hoveredIndex.row() == index.row()) {
            opt.state |= QStyle::State_MouseOver;
        } else {
            opt.state &= ~QStyle::State_MouseOver;
        }
    }

    if (!index.column()) {
        opt.viewItemPosition = QStyleOptionViewItemV4::Beginning;
    } else if (index.column() == index.model()->columnCount() - 1) {
        opt.viewItemPosition = QStyleOptionViewItemV4::End;
    } else {
        opt.viewItemPosition = QStyleOptionViewItemV4::Middle;
    }

    QStyledItemDelegate::paint(painter, opt, index);
}
