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
#include <QWheelEvent>
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QtDebug>
#include <cmath>
#include "TracksView.h"
#include "Media.h"
#include "Library.h"
#include "GraphicsMovieItem.h"
#include "GraphicsCursorItem.h"

TracksView::TracksView( QGraphicsScene* scene, MainWorkflow* mainWorkflow, QWidget* parent )
        : QGraphicsView( scene, parent ), m_scene( scene ), m_mainWorkflow( mainWorkflow )
{
    //TODO should be defined by the settings
    m_tracksHeight = 25;

    m_tracksCount = mainWorkflow->getTrackCount();
    m_fps = 30;

    m_numAudioTrack = 0;
    m_numVideoTrack = 0;

    setMouseTracking( true );
    setAcceptDrops( true );
    setContentsMargins( 0, 0, 0, 0 );
    setFrameStyle( QFrame::NoFrame );

    //// Adjust the height using the number of tracks
    //const int maxHeight = m_tracksHeight * m_tracksCount;
    //setSceneRect( 0, 0, sceneRect().width(), maxHeight );

    const int maxHeight = m_tracksHeight * 4;

    m_cursorLine = new GraphicsCursorItem( maxHeight, QPen( QColor( 220, 30, 30 ) ) );
    m_scene->addItem( m_cursorLine );

    createLayout();

    connect( m_cursorLine, SIGNAL( cursorPositionChanged(int) ),
             this, SLOT( ensureCursorVisible() ) );
}

void TracksView::createLayout()
{
    m_layout = new QGraphicsLinearLayout( Qt::Vertical );
    m_layout->setContentsMargins( 0, 0, 0, 0 );
    m_layout->setSpacing( 0 );

    QGraphicsWidget* container = new QGraphicsWidget();
    container->setContentsMargins( 0, 0, 0, 0 );
    container->setLayout( m_layout );

    addVideoTrack();
    addAudioTrack();

    m_scene->addItem( container );
}

void TracksView::addVideoTrack()
{
    GraphicsTrack* track = new GraphicsTrack();
    track->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    track->setPreferredHeight( m_tracksHeight );
    track->setContentsMargins( 0, 0, 0, 0 );
    m_layout->insertItem( 0, track );
    m_numVideoTrack++;
}

void TracksView::addAudioTrack()
{
    GraphicsTrack* track = new GraphicsTrack();
    track->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    track->setPreferredHeight( m_tracksHeight );
    track->setContentsMargins( 0, 0, 0, 0 );
    m_layout->insertItem( 1000, track );
    m_numAudioTrack++;
}

void TracksView::dragEnterEvent( QDragEnterEvent* event )
{
    if ( event->mimeData()->hasFormat( "vlmc/uuid" ) )
        event->acceptProposedAction();
}

void TracksView::dropEvent( QDropEvent* event )
{
    QUuid uuid = QUuid( (const QString& )event->mimeData()->data( "vlmc/uuid" ) );
    Media* media = Library::getInstance()->getMedia( uuid );
    if ( !media )
        return;

    addClip( media, event->pos() );

    event->acceptProposedAction();
}

void TracksView::dragMoveEvent( QDragMoveEvent* )
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
    /*QColor base = palette().button().color();
    QRectF r = rect;
    r.setWidth( r.width() + 1 );

    painter->setClipRect( r );
    painter->drawLine( r.left(), 0, r.right(), 0 );

    uint tracks = m_tracksCount;
    for ( uint i = 0; i < tracks; ++i )
        painter->drawLine( r.left(), m_tracksHeight * ( i + 1 ), r.right(), m_tracksHeight * ( i + 1 ) );

    int lowerLimit = m_tracksHeight * m_tracksCount + 1;
    if ( height() > lowerLimit )
        painter->fillRect( QRectF ( r.left(), lowerLimit, r.width(), height() - lowerLimit ), QBrush( base ) );*/
}

void TracksView::mouseMoveEvent( QMouseEvent* event )
{
    QGraphicsView::mouseMoveEvent( event );
}

void TracksView::mousePressEvent( QMouseEvent* event )
{

    /*QList<QGraphicsItem*> collisionList = items( event->pos() );

    if ( event->modifiers() == Qt::ControlModifier && collisionList.count() == 0 )
    {
        setDragMode( QGraphicsView::ScrollHandDrag );
        QGraphicsView::mousePressEvent( event );
        return;
    }

    if ( event->modifiers() & Qt::ShiftModifier && collisionList.count() == 0 )
    {
        setDragMode( QGraphicsView::RubberBandDrag );
        if ( !event->modifiers() & Qt::ControlModifier )
            scene()->clearSelection();
        QGraphicsView::mousePressEvent( event );
        return;
    }

    QGraphicsView::mousePressEvent( event );*/
}

void TracksView::mouseReleaseEvent( QMouseEvent* event )
{
    /*setDragMode( QGraphicsView::NoDrag );
    QGraphicsView::mouseReleaseEvent( event );*/
}

void TracksView::wheelEvent( QWheelEvent* event )
{
    if ( event->modifiers() == Qt::ControlModifier )
    {
        // CTRL + WHEEL = Zoom
        if ( event->delta() > 0 )
            emit zoomIn();
        else
            emit zoomOut();
        event->accept();
    }
    else
    {
        //TODO should scroll the timeline
        event->ignore();
        QGraphicsView::wheelEvent( event );
    }
}

void TracksView::setCursorPos( int pos )
{
    if ( pos < 0 ) pos = 0;
    m_cursorLine->setCursorPos( pos );
}

int TracksView::cursorPos()
{
    return m_cursorLine->cursorPos();
}

void TracksView::addClip( Media* clip, const QPoint& point )
{
    unsigned int track = (unsigned int)( mapToScene( point ).y() / m_tracksHeight );
    if ( track + 1 > m_tracksCount ) return;

    //mappedXPos: 1 pixel = 1 frame
    qreal mappedXPos = ( mapToScene( point ).x() + 0.5 );

    GraphicsMovieItem* item = new GraphicsMovieItem( clip );
    item->setPos( mappedXPos, track * tracksHeight() );
    item->setWidth( ( (double)clip->getLength() / 1000 ) * m_fps );
    item->setHeight( tracksHeight() );
    //item->setAudioSpectrum( clip->getAudioSpectrum() );
    m_scene->addItem( item );
    int duration = mappedXPos + ( (double)clip->getLength() / 1000 ) * m_fps;
    if ( duration > m_projectDuration )
    {
        m_projectDuration = duration;
        emit durationChanged( duration );
    }
    item->show();
    qDebug() << "TracksView::addClip: Adding a new clip to track" << track;
    //FIXME: this leaks, but it will be corrected once we really use Clip instead
    //          of Media
    m_mainWorkflow->addClip( new Clip( clip ), track, (qint64)mappedXPos );
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
    centerOn( m_cursorLine );
}

void TracksView::ensureCursorVisible()
{
    if ( horizontalScrollBar()->isVisible() )
    {
        QRectF r( m_cursorLine->boundingRect().width() / 2,
                  m_cursorLine->boundingRect().height() / 2,
                  1, 1 );
        m_cursorLine->ensureVisible( r, 150, 50 );
    }
}
