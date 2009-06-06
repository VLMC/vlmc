/*****************************************************************************
 * TracksView.h: QGraphicsView that contains the TracksScene
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

#ifndef TRACKSVIEW_H
#define TRACKSVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QWheelEvent>
#include <QGraphicsSceneDragDropEvent>
#include "Media.h"
#include "GraphicsCursorItem.h"
#include "Workflow/MainWorkflow.h"

class GraphicsMovieItem;

class GraphicsTrack : public QGraphicsWidget
{
    Q_OBJECT

public:
    GraphicsTrack( QGraphicsItem* parent = 0 ) : QGraphicsWidget( parent ) {}

protected:
    virtual void paint( QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* = 0 )
    {
        painter->setBrush( Qt::cyan );
        painter->drawRect( rect() );
    }
};

class TracksView : public QGraphicsView
{
    Q_OBJECT

public:
    TracksView( QGraphicsScene* scene, MainWorkflow* mainWorkflow, QWidget* parent = 0 );
    void setDuration( int duration );
    int duration() const { return m_projectDuration; }
    int tracksHeight() const { return m_tracksHeight; }
    unsigned int tracksCount() const { return m_tracksCount; }
    void setCursorPos( int pos );
    int cursorPos();
    GraphicsCursorItem* tracksCursor() const { return m_cursorLine; }
    //FIXME: this should probably take a Clip* as a parameter doesn't it ?
    void addClip( Media* clip, const QPoint& point );
    void setScale( double scaleFactor );

protected:
    virtual void            resizeEvent( QResizeEvent* event );
    virtual void            drawBackground( QPainter* painter, const QRectF& rect );
    virtual void            mouseMoveEvent( QMouseEvent* event );
    virtual void            mousePressEvent( QMouseEvent* event );
    virtual void            mouseReleaseEvent( QMouseEvent* event );
    virtual void            wheelEvent( QWheelEvent* event );
    virtual void            dragEnterEvent( QDragEnterEvent* event );
    virtual void            dragMoveEvent( QDragMoveEvent* event );
    virtual void            dragLeaveEvent( QDragLeaveEvent* event );
    virtual void            dropEvent( QDropEvent* event );

private slots:
    void                    ensureCursorVisible();

private:
    void                    createLayout();
    void                    addVideoTrack();
    void                    addAudioTrack();
    QGraphicsScene*         m_scene;
    int                     m_tracksHeight;
    unsigned int            m_tracksCount;
    int                     m_projectDuration;
    int                     m_fps;
    GraphicsCursorItem*     m_cursorLine;
    QGraphicsLinearLayout*  m_layout;
    int                     m_numVideoTrack;
    int                     m_numAudioTrack;
    MainWorkflow*           m_mainWorkflow;
    GraphicsMovieItem*      m_dragItem;

signals:
    void                    zoomIn();
    void                    zoomOut();
    void                    durationChanged( int duration );
};

#endif // TRACKSVIEW_H
