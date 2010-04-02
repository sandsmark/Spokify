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

#ifndef PLAYPAUSEBUTTON_H
#define PLAYPAUSEBUTTON_H

#include <QtGui/QWidget>

#include <QtGui/QPixmap>

class PlayPauseButton
    : public QWidget
{
    Q_OBJECT

public:
    PlayPauseButton(QWidget *parent = 0);
    virtual ~PlayPauseButton();

    virtual QSize sizeHint() const;

    void setIsPlaying(bool isPlaying);
    bool isPlaying() const;

Q_SIGNALS:
    void play();
    void pause();

protected:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

private:
    bool    m_hovered;
    bool    m_isPlaying;
    QPixmap m_play;
    QPixmap m_hoveredPlay;
    QPixmap m_pause;
    QPixmap m_hoveredPause;
};

#endif
