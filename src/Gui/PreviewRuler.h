/*****************************************************************************
 * PreviewRuler.h : Slider/Ruler used into the PreviewWidget
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

#include <QWidget>
#include <QPaintEvent>
#include "GenericRenderer.h"

#define MARK_XSMALL 3
#define MARK_SMALL 5
#define MARK_MEDIUM 8
#define MARK_LARGE 11

#define MARKER_DEFAULT -1

class PreviewRuler : public QWidget
{
    Q_OBJECT
public:
    enum Marker
    {
        START,
        STOP
    };

    PreviewRuler( QWidget* parent = 0 );
    virtual ~PreviewRuler() { }
    void setRenderer( GenericRenderer* renderer );
    void setMarker( Marker m );
    qint64 getMarker( Marker m ) const;

public slots:
    void setFrame( qint64 frame, bool broadcastEvent = false );

protected:
    virtual void paintEvent( QPaintEvent* event );
    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent * event );

private slots:
    void updateTimecode( qint64 frames = -1 );
    void clear();

private:
    GenericRenderer*    m_renderer;
    qint64              m_frame;
    qint64              m_markerStart;
    qint64              m_markerStop;
    bool                m_isSliding;

signals:
    void frameChanged( qint64, MainWorkflow::FrameChangedReason );
    void timeChanged( int h, int m, int s, int f );
};

#endif // PREVIEWRULER_H
