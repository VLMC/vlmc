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
#include <QGraphicsSceneDragDropEvent>
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
    m_dragItem = NULL;

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
    m_layout->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    m_layout->setContentsMargins( 0, 0, 0, 0 );
    m_layout->setSpacing( 0 );
    m_layout->setPreferredWidth( 0 );

    QGraphicsWidget* container = new QGraphicsWidget();
    container->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
    container->setContentsMargins( 0, 0, 0, 0 );
    container->setLayout( m_layout );

    // Create the initial layout
    // - 1 video track
    // - a separator
    // - 1 audio track
    addVideoTrack();

    m_separator = new QGraphicsWidget();
    m_separator->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    m_separator->setPreferredHeight( 20 );
    m_layout->insertItem( 1, m_separator );

    addAudioTrack();

    m_scene->addItem( container );
}

void TracksView::addVideoTrack()
{
    GraphicsTrack* track = new GraphicsTrack( GraphicsTrack::Video );
    track->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    track->setPreferredHeight( m_tracksHeight );
    track->setContentsMargins( 0, 0, 0, 0 );
    m_layout->insertItem( 0, track );
    m_numVideoTrack++;
    m_scene->invalidate();
    //FIXME this should maybe go elsewhere
    setSceneRect( m_layout->contentsRect().adjusted( 0, 0, 100, 100 ) );
}

void TracksView::addAudioTrack()
{
    GraphicsTrack* track = new GraphicsTrack( GraphicsTrack::Audio );
    track->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    track->setPreferredHeight( m_tracksHeight );
    track->setContentsMargins( 0, 0, 0, 0 );
    m_layout->insertItem( 1000, track );
    m_numAudioTrack++;
    m_scene->invalidate();
    //FIXME this should maybe go elsewhere
    setSceneRect( m_layout->contentsRect().adjusted( 0, 0, 100, 100 ) );
}

void TracksView::dragEnterEvent( QDragEnterEvent* event )
{
    if ( event->mimeData()->hasFormat( "vlmc/uuid" ) )
        event->acceptProposedAction();

    QUuid uuid = QUuid( (const QString& )event->mimeData()->data( "vlmc/uuid" ) );
    Media* media = Library::getInstance()->getMedia( uuid );
    if ( !media ) return;

    int track = (int)( mapToScene( event->pos() ).y() / m_tracksHeight );
    if ( track > m_numVideoTrack - 1 )
        track = m_numVideoTrack - 1;

    qreal mappedXPos = ( mapToScene( event->pos() ).x() + 0.5 );

    if ( m_dragItem ) delete m_dragItem;
    m_dragItem = new GraphicsMovieItem( media );
    m_dragItem->setPos( mappedXPos, 0 );
    m_dragItem->setWidth( ( (double)media->getLength() / 1000 ) * m_fps );
    m_dragItem->setHeight( tracksHeight() );
    m_dragItem->setParentItem( m_layout->itemAt( track )->graphicsItem() );
}

void TracksView::dragMoveEvent( QDragMoveEvent* event )
{
    if ( !m_dragItem ) return;

    int track = (unsigned int)( mapToScene( event->pos() ).y() / m_tracksHeight );
    if ( track > m_numVideoTrack - 1)
        track = m_numVideoTrack - 1;

    qreal mappedXPos = ( mapToScene( event->pos() ).x() + 0.5 );
    m_dragItem->setPos( mappedXPos, 0 );
    m_dragItem->setParentItem( m_layout->itemAt( track )->graphicsItem() );
}

void TracksView::dragLeaveEvent( QDragLeaveEvent* event )
{
    if ( m_dragItem )
    {
        delete m_dragItem;
        m_dragItem = NULL;
        updateDuration();
    }
}

void TracksView::dropEvent( QDropEvent* event )
{
    qDebug() << "Dropping is currently not implemented.";

    if ( m_dragItem )
    {
        updateDuration();
        if ( m_layout->itemAt( 0 )->graphicsItem()->childItems().count() > 0 )
            addVideoTrack();
        event->acceptProposedAction();
        //addClip( media, event->pos() );
        m_dragItem = NULL; // Temporary action
    }
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
    QRectF r = rect;
    r.setWidth( r.width() + 1 );

    painter->setBrush( palette().dark() );
    painter->setPen( Qt::transparent );
    painter->drawRect( r.left(), m_separator->y(),
                       r.right(),
                       m_separator->boundingRect().height() );

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
        painter->fillRect( QRectF ( r.left(), lowerLimit, r.width(),
        height() - lowerLimit ), QBrush( base ) );*/
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
        updateDuration();
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

void TracksView::updateDuration()
{
    QList<QGraphicsItem*> sceneItems = m_scene->items();

    int projectDuration = 0;
    for ( int i = 0; i < sceneItems.size(); ++i )
    {
        AbstractGraphicsMediaItem* item =
                dynamic_cast<AbstractGraphicsMediaItem*>( sceneItems.at( i ) );
        if ( !item ) continue;
        if ( item->pos().x() + item->boundingRect().width() > projectDuration )
            projectDuration = item->pos().x() + item->boundingRect().width();
    }

    m_projectDuration = projectDuration;

    // PreferredWidth not working ?
    m_layout->setMinimumWidth( m_projectDuration );
    m_layout->setMaximumWidth( m_projectDuration );

    emit durationChanged( m_projectDuration );
}
