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

#include "tabledelegate.h"

#include <QtGui/QTableView>

TableDelegate::TableDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

TableDelegate::~TableDelegate()
{
}

void TableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &_option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        QStyledItemDelegate::paint(painter, _option, index);
        return;
    }
    QStyleOptionViewItemV4 option(*qstyleoption_cast<const QStyleOptionViewItemV4*>(&_option));
    const QTableView *const view = qobject_cast<const QTableView*>(option.widget);
    const QModelIndex currIndex = view->indexAt(view->viewport()->mapFromGlobal(QCursor::pos()));
    if (currIndex.isValid() && currIndex.row() == index.row()) {
        option.state |= QStyle::State_MouseOver;
    } else {
        option.state &= ~QStyle::State_MouseOver;
    }
    if (!index.column()) {
        option.viewItemPosition = QStyleOptionViewItemV4::Beginning;
    } else if (index.column() == index.model()->columnCount() - 1) {
        option.viewItemPosition = QStyleOptionViewItemV4::End;
    } else {
        option.viewItemPosition = QStyleOptionViewItemV4::Middle;
    }
    QStyledItemDelegate::paint(painter, option, index);
}
