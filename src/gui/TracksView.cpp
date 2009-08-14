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
#include "Commands.hpp"

TracksView::TracksView( QGraphicsScene* scene, MainWorkflow* mainWorkflow, QWidget* parent )
        : QGraphicsView( scene, parent ), m_scene( scene ), m_mainWorkflow( mainWorkflow )
{
    //TODO should be defined by the settings
    m_tracksHeight = 25;

    m_tracksCount = mainWorkflow->getTrackCount();
    m_fps = FPS;

    m_numAudioTrack = 0;
    m_numVideoTrack = 0;
    m_dragItem = NULL;
    m_actionMove = false;
    m_actionRelativeX = -1;
    m_actionItem = NULL;

    setMouseTracking( true );
    setAcceptDrops( true );
    setContentsMargins( 0, 0, 0, 0 );
    setFrameStyle( QFrame::NoFrame );

    //TODO Remove the GraphicsCursorItem parameter height (not useful anymore)
    m_cursorLine = new GraphicsCursorItem( 1, QPen( QColor( 220, 30, 30 ) ) );

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
    GraphicsTrack* track = new GraphicsTrack( GraphicsTrack::Video, m_numVideoTrack );
    track->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    track->setPreferredHeight( m_tracksHeight );
    track->setContentsMargins( 0, 0, 0, 0 );
    m_layout->insertItem( 0, track );
    QApplication::processEvents(); //FIXME This is a bit hackish
    m_numVideoTrack++;
    m_scene->invalidate();
    //FIXME this should maybe go elsewhere
    setSceneRect( m_layout->contentsRect().adjusted( 0, 0, 100, 100 ) );
    m_cursorLine->setHeight( m_layout->contentsRect().height() );
}

void TracksView::addAudioTrack()
{
    GraphicsTrack* track = new GraphicsTrack( GraphicsTrack::Audio, 0 );
    track->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    track->setPreferredHeight( m_tracksHeight );
    track->setContentsMargins( 0, 0, 0, 0 );
    m_layout->insertItem( 1000, track );
    QApplication::processEvents(); //FIXME This is a bit hackish
    m_numAudioTrack++;
    m_scene->invalidate();
    //FIXME this should maybe go elsewhere
    setSceneRect( m_layout->contentsRect().adjusted( 0, 0, 100, 100 ) );
    m_cursorLine->setHeight( m_layout->contentsRect().height() );
}

void TracksView::dragEnterEvent( QDragEnterEvent* event )
{
    if ( event->mimeData()->hasFormat( "vlmc/uuid" ) )
        event->acceptProposedAction();

    QUuid uuid = QUuid( QString( event->mimeData()->data( "vlmc/uuid" ) ) );
    Clip* clip = Library::getInstance()->getClip( uuid );
    if ( !clip ) return;

    qreal mappedXPos = ( mapToScene( event->pos() ).x() + 0.5 );

    if ( m_dragItem ) delete m_dragItem;
    m_dragItem = new GraphicsMovieItem( clip );
    m_dragItem->setWidth( ( int ) ( clip->getLength() ) );
    m_dragItem->setHeight( tracksHeight() );
    m_dragItem->setPos( mappedXPos, 0 );
    m_dragItem->setParentItem( m_layout->itemAt( 0 )->graphicsItem() );
    moveMediaItem( m_dragItem, event->pos() );
}

void TracksView::dragMoveEvent( QDragMoveEvent* event )
{
    if ( !m_dragItem ) return;
    moveMediaItem( m_dragItem, event->pos() );
}

void TracksView::moveMediaItem( const QUuid& uuid, unsigned int track, qint64 time )
{
    QList<QGraphicsItem*> sceneItems = m_scene->items();

    for ( int i = 0; i < sceneItems.size(); ++i )
    {
        AbstractGraphicsMediaItem* item =
                dynamic_cast<AbstractGraphicsMediaItem*>( sceneItems.at( i ) );
        if ( !item || item->uuid() != uuid ) continue;
        moveMediaItem( item, track, time );
    }
}

void TracksView::moveMediaItem( AbstractGraphicsMediaItem* item, QPoint position )
{
    GraphicsTrack* track = NULL;
    QList<QGraphicsItem*> list = items( position );
    for ( int i = 0; i < list.size(); ++i )
    {
        track = qgraphicsitem_cast<GraphicsTrack*>( list.at(i) );
        if (track) break;
    }

    if (!track) return;

    qreal time = ( mapToScene( position ).x() + 0.5 );
    moveMediaItem( item, track->trackNumber(), (int)time);
}

void TracksView::moveMediaItem( AbstractGraphicsMediaItem* item, int track, int time )
{
    if ( track < 0 )
        track = 0;
    else if ( track > m_numVideoTrack - 1)
        track = m_numVideoTrack - 1;

    //qDebug() << ">>>>>> Move track number" << track;

    QPointF oldPos = item->pos();
    QGraphicsItem* oldParent = item->parentItem();
    // Check for vertical collisions
    item->setParentItem( m_layout->itemAt( m_numVideoTrack - track - 1 )->graphicsItem() );
    bool continueSearch = true;
    while ( continueSearch )
    {
        QList<QGraphicsItem*> colliding = item->collidingItems( Qt::IntersectsItemShape );
        bool itemCollision = false;
        for ( int i = 0; i < colliding.size(); ++i )
        {
            AbstractGraphicsMediaItem* currentItem = dynamic_cast<AbstractGraphicsMediaItem*>( colliding.at( i ) );
            if ( currentItem )
            {
                // Collision with an item of the same type
                itemCollision = true;
                if ( currentItem->trackNumber() > track )
                {
                    if ( track < 1 )
                    {
                        item->setParentItem( oldParent );
                        continueSearch = false;
                        break;
                    }
                    track -= 1;
                    Q_ASSERT( m_layout->itemAt( track )->graphicsItem() != NULL );
                    item->setParentItem( m_layout->itemAt( track )->graphicsItem() );
                }
                else if ( currentItem->trackNumber() < track )
                {
                    if ( track >= m_numVideoTrack - 1 )
                    {
                        item->setParentItem( oldParent );
                        continueSearch = false;
                        break;
                    }
                    track += 1;
                    Q_ASSERT( m_layout->itemAt( track )->graphicsItem() != NULL );
                    item->setParentItem( m_layout->itemAt( track )->graphicsItem() );
                }
            }
        }
        if ( !itemCollision )
            continueSearch = false;
    }
    // Check for horizontal collisions
    int mappedXPos = qMax( time, 0 );
    item->setPos( mappedXPos, 0 );
    QList<QGraphicsItem*> colliding = item->collidingItems( Qt::IntersectsItemShape );
    for ( int i = 0; i < colliding.size(); ++i )
    {
        AbstractGraphicsMediaItem* currentItem = dynamic_cast<AbstractGraphicsMediaItem*>( colliding.at( i ) );
        if ( currentItem )
        {
            // Collision with an item of the same type
            // Restoring original position (horizontal)
            if ( oldPos.isNull() == false )
                item->setPos( oldPos );
            break;
        }
    }
}

void TracksView::dragLeaveEvent( QDragLeaveEvent* event )
{
    Q_UNUSED( event )
    if ( m_dragItem )
    {
        delete m_dragItem;
        m_dragItem = NULL;
        updateDuration();
    }
}

void TracksView::dropEvent( QDropEvent* event )
{
    if ( m_dragItem )
    {
        updateDuration();
        if ( m_layout->itemAt( 0 )->graphicsItem()->childItems().count() > 0 )
            addVideoTrack();
        event->acceptProposedAction();

        qreal mappedXPos = ( mapToScene( event->pos() ).x() + 0.5 );
        m_dragItem->oldTrackNumber = m_dragItem->trackNumber();
        Commands::trigger( new Commands::MainWorkflow::AddClip( m_mainWorkflow,
                                                                m_dragItem->clip(),
                                                                m_dragItem->trackNumber(),
                                                                (qint64)mappedXPos ) );
        m_dragItem = NULL;
    }
}

void TracksView::setDuration( int duration )
{
    int diff = ( int ) qAbs( ( qreal )duration - sceneRect().width() );
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

    painter->setBrush( QBrush( palette().dark().color(), Qt::Dense3Pattern ) );
    painter->setPen( Qt::transparent );
    painter->drawRect( ( int ) r.left(), ( int ) m_separator->y(),
                       ( int ) r.right(),
                       ( int ) m_separator->boundingRect().height() );

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
    if ( event->modifiers() == Qt::NoModifier &&
         event->buttons() == Qt::LeftButton &&
         m_actionMove == true )
    {
        if ( m_actionRelativeX < 0 )
            m_actionRelativeX = event->pos().x() - mapFromScene( m_actionItem->pos() ).x();
        moveMediaItem( m_actionItem, QPoint( event->pos().x() - m_actionRelativeX, event->pos().y() ) );
    }

    QGraphicsView::mouseMoveEvent( event );
}

void TracksView::mousePressEvent( QMouseEvent* event )
{
    QList<AbstractGraphicsMediaItem*> mediaCollisionList = mediaItems( event->pos() );

    if ( event->modifiers() == Qt::ControlModifier && mediaCollisionList.count() == 0 )
    {
        setDragMode( QGraphicsView::ScrollHandDrag );
        QGraphicsView::mousePressEvent( event );
        return;
    }

    if ( event->modifiers() == Qt::NoModifier &&
         event->button() == Qt::LeftButton &&
         mediaCollisionList.count() == 1 )
    {
        AbstractGraphicsMediaItem* item = mediaCollisionList.at( 0 );
        if ( item->moveable() )
        {
            m_actionMove = true;
            m_actionItem = mediaCollisionList.at( 0 );
        }
        return;
    }

    /*if ( event->modifiers() & Qt::ShiftModifier && collisionList.count() == 0 )
    {
        setDragMode( QGraphicsView::RubberBandDrag );
        if ( !event->modifiers() & Qt::ControlModifier )
            scene()->clearSelection();
        QGraphicsView::mousePressEvent( event );
        return;
    }*/

    QGraphicsView::mousePressEvent( event );
}

void TracksView::mouseReleaseEvent( QMouseEvent* event )
{
    if ( m_actionMove )
    {
        GraphicsMovieItem* movieItem = qgraphicsitem_cast<GraphicsMovieItem*>( m_actionItem );
        if ( movieItem )
        {
            updateDuration();
            if ( m_layout->itemAt( 0 )->graphicsItem()->childItems().count() > 0 )
                addVideoTrack();
            qDebug() << "Trigerring move command. track" << movieItem->oldTrackNumber << "->" << movieItem->trackNumber();
            Commands::trigger( new Commands::MainWorkflow::MoveClip( m_mainWorkflow, movieItem->clip()->getUuid(),
                                                                     movieItem->oldTrackNumber, movieItem->trackNumber(),
                                                                     (qint64)movieItem->pos().x() ) );
            movieItem->oldTrackNumber = movieItem->trackNumber();
            m_actionMove = false;
            m_actionRelativeX = -1;
            m_actionItem = NULL;
        }
    }

    setDragMode( QGraphicsView::NoDrag );
    QGraphicsView::mouseReleaseEvent( event );
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

QList<AbstractGraphicsMediaItem*> TracksView::mediaItems( const QPoint& pos )
{
    QList<QGraphicsItem*> collisionList = items( pos );
    QList<AbstractGraphicsMediaItem*> mediaCollisionList;
    for ( int i = 0; i < collisionList.size(); ++i )
    {
        AbstractGraphicsMediaItem* item =
                dynamic_cast<AbstractGraphicsMediaItem*>( collisionList.at( i ) );
        if ( item )
            mediaCollisionList.append( item );
    }
    return mediaCollisionList;
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

void TracksView::setScale( double scaleFactor )
{
    QMatrix matrix;
    matrix.scale( scaleFactor, 1 );
    //TODO update the scene scale ?
    setMatrix( matrix );

    int diff = ( int ) ( sceneRect().width() - ( qreal ) m_projectDuration );
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
    //TODO this should use a variant of mediaItems( const QPoint& )
    QList<QGraphicsItem*> sceneItems = m_scene->items();

    int projectDuration = 0;
    for ( int i = 0; i < sceneItems.size(); ++i )
    {
        AbstractGraphicsMediaItem* item =
                dynamic_cast<AbstractGraphicsMediaItem*>( sceneItems.at( i ) );
        if ( !item ) continue;
        if ( item->pos().x() + item->boundingRect().width() > projectDuration )
            projectDuration = ( int ) ( item->pos().x() + item->boundingRect().width() );
    }

    m_projectDuration = projectDuration;

    // PreferredWidth not working ?
    m_layout->setMinimumWidth( m_projectDuration );
    m_layout->setMaximumWidth( m_projectDuration );

    emit durationChanged( m_projectDuration );
}
