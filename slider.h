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
    Q_OBJECT

public:
    Slider(QWidget *parent = 0);
    virtual ~Slider();

    virtual QSize sizeHint() const;

    void setRange(quint64 minimum, quint64 maximum);
    quint64 maximum() const;
    quint64 minimum() const;

    void setValue(quint64 value);
    quint64 value() const;

    void setCacheValue(quint64 value);
    quint64 cacheValue() const;

Q_SIGNALS:
    void sliderReleased();
    void maximumReached();

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QPixmap m_leftBackground;
    QPixmap m_leftForeground;
    QPixmap m_rightBackground;
    QPixmap m_rightForeground;
    QPixmap m_bodyBackground;
    QPixmap m_bodyForeground;
    QPixmap m_slider;
    QPixmap m_disabledSlider;

    quint64 m_minimum;
    quint64 m_maximum;
    quint64 m_value;
    quint64 m_cacheValue;
};

#endif