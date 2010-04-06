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
    setFixedSize(48, 48);

    const QString startPath = KStandardDirs::locate("appdata", "images/media-playback-start.png");
    const QString pausePath = KStandardDirs::locate("appdata", "images/media-playback-pause.png");

    m_play = QImage(startPath);
    m_pause = QImage(pausePath);

    m_hoveredPlay = m_play;
    KIconEffect::toGamma(m_hoveredPlay, 0.5);

    m_disabledPlay = m_play;
    KIconEffect::semiTransparent(m_disabledPlay);

    m_hoveredPause = m_pause;
    KIconEffect::toGamma(m_hoveredPause, 0.5);

    m_disabledPause = m_pause;
    KIconEffect::semiTransparent(m_disabledPause);
}

PlayPauseButton::~PlayPauseButton()
{
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

    QImage image;

    if (m_isPlaying) {
        if (isEnabled()) {
            if (m_hovered) {
                image = m_hoveredPause;
            } else {
                image = m_pause;
            }
        } else {
            image = m_disabledPause;
        }
    } else {
        if (isEnabled()) {
            if (m_hovered) {
                image = m_hoveredPlay;
            } else {
                image = m_play;
            }
        } else {
            image = m_disabledPlay;
        }
    }

    QPainter p(this);
    p.drawImage(0, 0, image);
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
