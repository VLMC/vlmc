/*****************************************************************************
 * PreviewRuler.h : Slider/Ruler used into the PreviewWidget
 * with backward compatibility with QAbstractSlider.
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Ludovic Fauvet <etix@l0cal.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef PREVIEWRULER_H
#define PREVIEWRULER_H

#include <QAbstractSlider>
#include <QPaintEvent>
#include <QPainter>
#include "GenericRenderer.h"

#define MARK_XSMALL 3
#define MARK_SMALL 5
#define MARK_MEDIUM 8
#define MARK_LARGE 11

class PreviewRuler : public QAbstractSlider
{
    Q_OBJECT
public:
    PreviewRuler( QWidget* parent = 0 );
    virtual ~PreviewRuler() { }
    void setRenderer( GenericRenderer* renderer );

public slots:
    void setFrame( qint64 frame );

protected:
    virtual void paintEvent( QPaintEvent* event );
    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent * event );
    virtual void sliderChange( SliderChange change );

private slots:
    void positionChanged();

private:
    GenericRenderer* m_renderer;
    qint64 m_frame;
    bool m_isSliding;
    int m_range;

signals:
    void frameChanged( qint64 );
    void sliderPosChanged( int value );
    void timeChanged( int h, int m, int s, int f );
};

#endif // PREVIEWRULER_H
