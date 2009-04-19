/*****************************************************************************
 * TracksView.cpp: QGraphicsView that contains the TracksScene
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

#include <QScrollBar>
#include <QMouseEvent>
#include <QtDebug>
#include <cmath>
#include "TracksView.h"
#include "Clip.h"
#include "Library.h"
#include "GraphicsMovieItem.h"

TracksView::TracksView( QGraphicsScene* scene, QWidget* parent )
        : QGraphicsView( scene, parent ), m_scene( scene )
{
    //TODO should be defined by the settings
    m_tracksHeight = 50;

    //TODO should be dynamic
    m_tracksCount = 5;
    m_fps = 30;

    setMouseTracking( true );
    setAcceptDrops( true );
    setDragMode( QGraphicsView::ScrollHandDrag );
    setContentsMargins( 0, 0, 0, 0 );

    // Adjust the height using the number of tracks
    const int maxHeight = m_tracksHeight * m_tracksCount;
    setSceneRect( 0, 0, sceneRect().width(), maxHeight );

    m_cursorPos = 0;
    m_cursorLine = m_scene->addLine(0, 0, 0, maxHeight);
    m_cursorLine->setZValue(100);
}

void TracksView::dragEnterEvent( QDragEnterEvent* event )
{
    if ( event->mimeData()->hasFormat( "vlmc/uuid" ) )
        event->acceptProposedAction();
}

void TracksView::dropEvent( QDropEvent* event )
{
    QUuid uuid = QUuid( (const QString& )event->mimeData()->data( "vlmc/uuid" ) );
    Clip* clip = Library::getInstance()->getClip( uuid );
    if ( !clip ) return;

    addClip( clip, event->pos() );

    event->acceptProposedAction();
}

void TracksView::dragMoveEvent( QDragMoveEvent* event )
{

}

void TracksView::setDuration( int duration )
{
    int diff = qAbs( duration - sceneRect().width() );
    if ( diff * matrix().m11() > -50 )
    {
        if ( matrix().m11() < 0.4 )
            setSceneRect( 0, 0, ( duration + 100 / matrix().m11() ), sceneRect().height() );
        else
            setSceneRect( 0, 0, ( duration + 300 ), sceneRect().height() );
    }
    m_projectDuration = duration;
}

void TracksView::resizeEvent( QResizeEvent* event )
{
    QGraphicsView::resizeEvent( event );
}

void TracksView::drawBackground( QPainter* painter, const QRectF& rect )
{
    QColor base = palette().button().color();
    QRectF r = rect;
    r.setWidth( r.width() + 1 );

    painter->setClipRect( r );
    painter->drawLine( r.left(), 0, r.right(), 0 );

    uint tracks = m_tracksCount;
    for ( uint i = 0; i < tracks; ++i )
        painter->drawLine( r.left(), m_tracksHeight * ( i + 1 ), r.right(), m_tracksHeight * ( i + 1 ) );

    int lowerLimit = m_tracksHeight * m_tracksCount + 1;
    if ( height() > lowerLimit )
        painter->fillRect( QRectF ( r.left(), lowerLimit, r.width(), height() - lowerLimit ), QBrush( base ) );
}

void TracksView::mouseMoveEvent( QMouseEvent* event )
{
    //int xPos = event->x();
    int mappedXPos = ( int )( mapToScene( event->pos() ).x() + 0.5 );
    if ( event->buttons() == Qt::LeftButton && event->modifiers() == Qt::NoModifier )
    {
        setCursorPos( mappedXPos );
    }

    QGraphicsView::mouseMoveEvent( event );
}

void TracksView::mousePressEvent( QMouseEvent* event )
{
    //int xPos = event->x();
    int mappedXPos = ( int )( mapToScene( event->pos() ).x() + 0.5 );
    if ( event->buttons() == Qt::LeftButton && event->modifiers() == Qt::NoModifier )
    {
        setCursorPos( mappedXPos );
    }

    QGraphicsView::mousePressEvent( event );
}

void TracksView::setCursorPos(int pos)
{
    m_cursorPos = pos;
    m_cursorLine->setPos(m_cursorPos, 0);
}

void TracksView::addClip( Clip* clip, const QPoint& point )
{
    int track = (int)( mapToScene( point ).y() / m_tracksHeight );
    if ( track + 1 > m_tracksCount ) return;

    //mappedXPos: 1 pixel = 1 frame
    int mappedXPos = ( int )( mapToScene( point ).x() + 0.5 );

    GraphicsMovieItem* item = new GraphicsMovieItem( clip );
    item->setPos( mappedXPos, track * tracksHeight() );
    item->setWidth( clip->getLength() / m_fps );
    item->setHeight( tracksHeight() );
    m_scene->addItem( item );
    item->show();
}

void TracksView::setScale( double scaleFactor )
{
    QMatrix matrix;
    matrix.scale( scaleFactor, 1 );
    //TODO update the scene scale ?
    setMatrix( matrix );

    int diff = sceneRect().width() - m_projectDuration;
    if ( diff * matrix.m11() < 50 )
    {
        if ( matrix.m11() < 0.4 )
            setSceneRect( 0, 0, ( m_projectDuration + 100 / matrix.m11() ), sceneRect().height() );
        else
            setSceneRect( 0, 0, ( m_projectDuration + 300 ), sceneRect().height() );
    }
}
