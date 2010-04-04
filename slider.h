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

#ifndef SLIDER_H
#define SLIDER_H

#include <QtGui/QWidget>

class Slider
    : public QWidget
{
public:
    Slider(QWidget *parent = 0);
    virtual ~Slider();

    virtual QSize sizeHint() const;

    void setRange(int minimum, int maximum);
    int maximum() const;
    int minimum() const;

    void setValue(int value);
    int value() const;

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QPixmap m_leftBackground;
    QPixmap m_rightBackground;
    QPixmap m_bodyBackground;
    QPixmap m_slider;
    QPixmap m_disabledSlider;

    int     m_minimum;
    int     m_maximum;
    int     m_value;
};

#endif