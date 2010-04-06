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

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QFontMetrics>

#include <KLocale>
#include <KIconEffect>
#include <KApplication>
#include <KStandardDirs>

TrackView::TrackView(QWidget *parent)
    : QTableView(parent)
    , m_information(KStandardDirs::locate("appdata", "images/dialog-information.png"))
    , m_disabledInformation(KStandardDirs::locate("appdata", "images/dialog-information.png"))
{
    viewport()->setMouseTracking(true);

    KIconEffect::semiTransparent(m_disabledInformation);
}

TrackView::~TrackView()
{
}

void TrackView::paintEvent(QPaintEvent *event)
{
    if (!model()->rowCount()) {
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
