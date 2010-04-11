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

#include "trackview.h"
#include "mimedata.h"
#include "trackmodel.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QFontMetrics>

#include <KLocale>
#include <KIconEffect>
#include <KApplication>
#include <KStandardDirs>

struct sp_track;

TrackView::TrackView(QWidget *parent)
    : QTableView(parent)
    , m_information(KStandardDirs::locate("appdata", "images/dialog-information.png"))
    , m_disabledInformation(KStandardDirs::locate("appdata", "images/dialog-information.png"))
    , m_searching(KStandardDirs::locate("appdata", "images/searching.png"))
    , m_isSearching(false)
{
    viewport()->setMouseTracking(true);

    KIconEffect::semiTransparent(m_disabledInformation);
}

TrackView::~TrackView()
{
}

void TrackView::setSearching(bool searching)
{
    m_isSearching = searching;
    update();
}

void TrackView::paintEvent(QPaintEvent *event)
{
    if (m_isSearching) {
        QPainter p(viewport());
        QFont f(kapp->font());
        f.setBold(true);
        f.setPointSize(f.pointSize() + 4);
        QFontMetrics fm(f);
        const QRect r = event->rect();
        if (isEnabled()) {
            p.drawImage(r.width() / 2 - m_information.width() / 2,
                        r.height() / 2 - m_information.height() / 2 - fm.height(),
                        m_searching);
        }
        QRect textRect = event->rect();
        textRect.setTop(textRect.top() + m_information.height() / 2 + textRect.height() / 2 - fm.height());
        p.setFont(f);
        p.drawText(textRect, Qt::AlignHCenter | Qt::AlignTop, i18n("Searching..."));
        return;
    } else if (!model() || !model()->rowCount()) {
        QPainter p(viewport());
        QFont f(kapp->font());
        f.setBold(true);
        f.setPointSize(f.pointSize() + 4);
        QFontMetrics fm(f);
        const QRect r = event->rect();
        if (isEnabled()) {
            p.drawImage(r.width() / 2 - m_information.width() / 2,
                        r.height() / 2 - m_information.height() / 2 - fm.height(),
                        m_information);
        } else {
            p.drawImage(r.width() / 2 - m_information.width() / 2,
                        r.height() / 2 - m_information.height() / 2 - fm.height(),
                        m_disabledInformation);
        }
        QRect textRect = event->rect();
        textRect.setTop(textRect.top() + m_information.height() / 2 + textRect.height() / 2 - fm.height());
        p.setFont(f);
        p.drawText(textRect, Qt::AlignHCenter | Qt::AlignTop, i18n("Empty track list"));
        return;
    }

    QTableView::paintEvent(event);
}

void TrackView::mouseMoveEvent(QMouseEvent *event)
{
    const QModelIndex hovered = indexAt(viewport()->mapFromGlobal(QCursor::pos()));
    QRect r(0, 0, 0, 0);
    if (hovered.isValid()) {
        r = visualRect(hovered);
        r.setLeft(0);
        r.setWidth(viewport()->width());
        viewport()->update(m_lastHovered.united(r));
    } else {
        viewport()->update();
    }
    m_lastHovered = r;

    QTableView::mouseMoveEvent(event);
}

void TrackView::enterEvent(QEvent *event)
{
    viewport()->update();
    QTableView::enterEvent(event);
}

void TrackView::leaveEvent(QEvent *event)
{
    viewport()->update();
    QTableView::leaveEvent(event);
}

void TrackView::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions);

    m_mimeData = new MimeData;
    m_mimeData->setTrack(currentIndex().data(TrackModel::SpotifyNativeTrackRole).value<sp_track*>());

    QDrag *drag = new QDrag(this);
    drag->setMimeData(m_mimeData);

    drag->exec(Qt::CopyAction); 
}
