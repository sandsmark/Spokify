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

#include "playpausebutton.h"

#include <QtGui/QImage>
#include <QtGui/QPainter>

#include <KIconEffect>
#include <KStandardDirs>

PlayPauseButton::PlayPauseButton(QWidget *parent)
    : QWidget(parent)
    , m_hovered(false)
    , m_isPlaying(false)
{
    const QString startPath = KStandardDirs::locate("appdata", "images/media-playback-start.png");
    const QString pausePath = KStandardDirs::locate("appdata", "images/media-playback-pause.png");

    m_play = QPixmap::fromImage(QImage(startPath));
    m_pause = QPixmap::fromImage(QImage(pausePath));

    QImage play(startPath);
    KIconEffect::toGamma(play, 0.5);
    m_hoveredPlay = QPixmap::fromImage(play);

    QImage disabledPlay(startPath);
    KIconEffect::semiTransparent(disabledPlay);
    m_disabledPlay = QPixmap::fromImage(disabledPlay);

    QImage pause(pausePath);
    KIconEffect::toGamma(pause, 0.5);
    m_hoveredPause = QPixmap::fromImage(pause);

    QImage disabledPause(pausePath);
    KIconEffect::semiTransparent(disabledPause);
    m_disabledPause = QPixmap::fromImage(disabledPause);
}

PlayPauseButton::~PlayPauseButton()
{
}

QSize PlayPauseButton::sizeHint() const
{
    return QSize(48, 48);
}

void PlayPauseButton::setIsPlaying(bool isPlaying)
{
    m_isPlaying = isPlaying;
    update();
}

bool PlayPauseButton::isPlaying() const
{
    return m_isPlaying;
}

void PlayPauseButton::enterEvent(QEvent *event)
{
    m_hovered = true;
    QWidget::enterEvent(event);
    update();
}

void PlayPauseButton::leaveEvent(QEvent *event)
{
    m_hovered = false;
    QWidget::leaveEvent(event);
    update();
}

void PlayPauseButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPixmap pixmap;

    if (m_isPlaying) {
        if (isEnabled()) {
            if (m_hovered) {
                pixmap = m_hoveredPause;
            } else {
                pixmap = m_pause;
            }
        } else {
            pixmap = m_disabledPause;
        }
    } else {
        if (isEnabled()) {
            if (m_hovered) {
                pixmap = m_hoveredPlay;
            } else {
                pixmap = m_play;
            }
        } else {
            pixmap = m_disabledPlay;
        }
    }

    QPainter p(this);
    p.drawPixmap(0, 0, pixmap);
}

void PlayPauseButton::mousePressEvent(QMouseEvent *event)
{
    if (!isEnabled()) {
        return;
    }

    if (m_isPlaying) {
        emit pause();
    } else {
        emit play();
    }

    m_isPlaying = !m_isPlaying;

    QWidget::mousePressEvent(event);
    update();
}
