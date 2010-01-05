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
#include "TracksView.h"
#include "Library.h"
#include "GraphicsMovieItem.h"
#include "GraphicsCursorItem.h"
#include "Commands.h"
#include "GraphicsTrack.hpp"
#include "WorkflowRenderer.h"

TracksView::TracksView( QGraphicsScene* scene, MainWorkflow* mainWorkflow, WorkflowRenderer* renderer, QWidget* parent )
        : QGraphicsView( scene, parent ), m_scene( scene ), m_mainWorkflow( mainWorkflow ),
        m_renderer( renderer )
{
    //TODO should be defined by the settings
    m_tracksHeight = 25;

    m_tracksCount = mainWorkflow->getTrackCount( MainWorkflow::VideoTrack );

    m_numAudioTrack = 0;
    m_numVideoTrack = 0;
    m_dragItem = NULL;
    m_actionMove = false;
    m_actionResize = false;
    m_actionRelativeX = -1;
    m_actionItem = NULL;
    m_tool = TOOL_DEFAULT;

    setMouseTracking( true );
    setAcceptDrops( true );
    setContentsMargins( 0, 0, 0, 0 );
    setFrameStyle( QFrame::NoFrame );
    setAlignment( Qt::AlignLeft | Qt::AlignTop );

    m_cursorLine = new GraphicsCursorItem( QPen( QColor( 220, 30, 30 ) ) );

    m_scene->addItem( m_cursorLine );

    connect( m_cursorLine, SIGNAL( cursorPositionChanged(qint64) ),
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

    setSceneRect( m_layout->contentsRect() );
}

void TracksView::addVideoTrack()
{
    GraphicsTrack* track = new GraphicsTrack( MainWorkflow::VideoTrack, m_numVideoTrack );
    track->setHeight( m_tracksHeight );
    m_layout->insertItem( 0, track );
    m_layout->activate();
    m_cursorLine->setHeight( m_layout->contentsRect().height() );
    m_scene->invalidate(); // Redraw the background
    m_numVideoTrack++;
    emit videoTrackAdded( track );
}

void TracksView::addAudioTrack()
{
    GraphicsTrack* track = new GraphicsTrack( MainWorkflow::AudioTrack, m_numAudioTrack );
    track->setHeight( m_tracksHeight );
    m_layout->insertItem( 1000, track );
    m_layout->activate();
    m_cursorLine->setHeight( m_layout->contentsRect().height() );
    m_scene->invalidate(); // Redraw the background
    m_numAudioTrack++;
    emit audioTrackAdded( track );
}

void TracksView::clear()
{
    m_layout->removeItem( m_separator );

    while ( m_layout->count() > 0 )
        delete m_layout->itemAt( 0 );

    m_layout->addItem( m_separator );

    m_numAudioTrack = 0;
    m_numVideoTrack = 0;

    addVideoTrack();
    addAudioTrack();

    updateDuration();
}

void TracksView::addMediaItem( Clip* clip, unsigned int track, qint64 start )
{
    Q_ASSERT( clip );

    // If there is not enough tracks to insert
    // the clip do it now.
    if ( track > (quint32)m_numVideoTrack )
    {
        unsigned int nbTrackToAdd = track - m_numVideoTrack;
        for ( unsigned int i = 0; i < nbTrackToAdd; ++i )
            addVideoTrack();
    }
    // Add the empty upper track
    if ( track + 1 == m_numVideoTrack )
        addVideoTrack();

    // Is the clip already existing in the timeline ?
    //TODO: please optimize me!
    QList<QGraphicsItem*> sceneItems = m_scene->items();
    for ( int i = 0; i < sceneItems.size(); ++i )
    {
        AbstractGraphicsMediaItem* item =
                dynamic_cast<AbstractGraphicsMediaItem*>( sceneItems.at( i ) );
        if ( !item || item->uuid() != clip->getUuid() ) continue;
        // Item already exist: goodbye!
        return;
    }

    GraphicsMovieItem* item = new GraphicsMovieItem( clip );
    item->m_tracksView = this;
    item->setHeight( tracksHeight() );
    item->setParentItem( getTrack( MainWorkflow::VideoTrack, track ) );
    item->setStartPos( start );
    item->oldTrackNumber = track;
    item->oldPosition = start;
    connect( item, SIGNAL( split(GraphicsMovieItem*,qint64) ),
             this, SLOT( split(GraphicsMovieItem*,qint64) ) );
    moveMediaItem( item, track, start );

    updateDuration();
}

void TracksView::dragEnterEvent( QDragEnterEvent* event )
{
    if ( event->mimeData()->hasFormat( "vlmc/uuid" ) )
        event->acceptProposedAction();

    QUuid uuid = QUuid( QString( event->mimeData()->data( "vlmc/uuid" ) ) );
    Clip* clip = Library::getInstance()->getClip( uuid );
    if ( !clip ) return;

    //FIXME: this leaks, but at least we have independant clips.
    clip = new Clip( clip );

    if ( m_dragItem ) delete m_dragItem;
    m_dragItem = new GraphicsMovieItem( clip );
    m_dragItem->m_tracksView = this;
    m_dragItem->setHeight( tracksHeight() );
    m_dragItem->setParentItem( m_layout->itemAt( 0 )->graphicsItem() );
    connect( m_dragItem, SIGNAL( split(GraphicsMovieItem*,qint64) ),
             this, SLOT( split(GraphicsMovieItem*,qint64) ) );
    moveMediaItem( m_dragItem, event->pos() );
}

void TracksView::dragMoveEvent( QDragMoveEvent* event )
{
    if ( !m_dragItem ) return;
    moveMediaItem( m_dragItem, event->pos() );
}

bool TracksView::setItemOldTrack( const QUuid &uuid, uint32_t oldTrackNumber )
{
    QList<QGraphicsItem*> sceneItems = m_scene->items();

    for ( int i = 0; i < sceneItems.size(); ++i )
    {
        AbstractGraphicsMediaItem* item =
                dynamic_cast<AbstractGraphicsMediaItem*>( sceneItems.at( i ) );
        if ( !item || item->uuid() != uuid ) continue;
        item->oldTrackNumber = oldTrackNumber;
        return true;
    }
    return false;
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
    static GraphicsTrack* lastKnownTrack = NULL;
    GraphicsTrack* track = NULL;

    QList<QGraphicsItem*> list = items( 0, position.y() );
    for ( int i = 0; i < list.size(); ++i )
    {
        track = qgraphicsitem_cast<GraphicsTrack*>( list.at(i) );
        if (track) break;
    }

    if ( !track )
    {
        // When the mouse pointer is not on a track,
        // use the last known track.
        // This avoids "breaks" when moving a rush
        if ( !lastKnownTrack ) return;
        track = lastKnownTrack;
    }

    lastKnownTrack = track;

    qreal time = ( mapToScene( position ).x() + 0.5 );
    moveMediaItem( item, track->trackNumber(), (qint64)time);
}

void TracksView::moveMediaItem( AbstractGraphicsMediaItem* item, quint32 track, qint64 time )
{
    if ( track > m_numVideoTrack - 1)
        track = m_numVideoTrack - 1;

    qint64 oldPos = item->startPos();
    QGraphicsItem* oldParent = item->parentItem();
    // Check for vertical collisions
    item->setParentItem( getTrack( MainWorkflow::VideoTrack, track ) );
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
                if ( currentItem->trackNumber() >= track )
                {
                    if ( track < 1 )
                    {
                        item->setParentItem( oldParent );
                        continueSearch = false;
                        break;
                    }
                    track -= 1;
                    Q_ASSERT( m_layout->itemAt( track )->graphicsItem() != NULL );
                    item->setParentItem( getTrack( MainWorkflow::VideoTrack, track ) );
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
                    item->setParentItem( getTrack( MainWorkflow::VideoTrack, track ) );
                }
            }
        }
        if ( !itemCollision )
            continueSearch = false;
    }
    // Check for horizontal collisions
    qint64 mappedXPos = qMax( time, (qint64)0 );
    item->setStartPos( mappedXPos );

    AbstractGraphicsMediaItem* hItem = NULL;
    QList<QGraphicsItem*> collide = item->collidingItems();
    for ( int i = 0; i < collide.count(); ++i )
    {
        hItem = dynamic_cast<AbstractGraphicsMediaItem*>( collide.at( i ) );
        if ( hItem ) break;
    }

    if ( hItem )
    {
        qreal newpos;
        // Evaluate a possible solution
        if ( item->pos().x() > hItem->pos().x() )
            newpos = hItem->pos().x() + hItem->boundingRect().width();
        else
            newpos = hItem->pos().x() - item->boundingRect().width();

        if ( newpos < 0 || newpos == hItem->pos().x() )
            item->setStartPos( oldPos ); // Fail
        else
        {
            // A solution may be found
            item->setStartPos( qRound64( newpos ) );
            QList<QGraphicsItem*> collideAgain = item->collidingItems();
            for ( int i = 0; i < collideAgain.count(); ++i )
            {
                AbstractGraphicsMediaItem* currentItem =
                        dynamic_cast<AbstractGraphicsMediaItem*>( collideAgain.at( i ) );
                if ( currentItem )
                {
                    item->setStartPos( oldPos ); // Fail
                    break;
                }
            }
        }
    }
}

void TracksView::removeMediaItem( const QUuid& uuid, unsigned int track )
{
    QList<QGraphicsItem*> trackItems = getTrack( MainWorkflow::VideoTrack, track )->childItems();;

    for ( int i = 0; i < trackItems.size(); ++i )
    {
        AbstractGraphicsMediaItem* item =
                dynamic_cast<AbstractGraphicsMediaItem*>( trackItems.at( i ) );
        if ( !item || item->uuid() != uuid ) continue;
        removeMediaItem( item );
    }
}

void TracksView::removeMediaItem( AbstractGraphicsMediaItem* item )
{
    QList<AbstractGraphicsMediaItem*> items;
    items.append( item );
    removeMediaItem( items );
}

void TracksView::removeMediaItem( const QList<AbstractGraphicsMediaItem*>& items )
{
    bool needUpdate = false;
    for ( int i = 0; i < items.size(); ++i )
    {
        GraphicsMovieItem* movieItem = qgraphicsitem_cast<GraphicsMovieItem*>( items.at( i ) );
        if ( !movieItem )
        {
            //TODO add support for audio tracks
            qWarning() << tr( "Action not supported." );
            continue;
        }

        delete movieItem;
        needUpdate = true;
    }

    if ( needUpdate ) updateDuration();
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
        m_dragItem->oldPosition = (qint64)mappedXPos;

        Commands::trigger( new Commands::MainWorkflow::AddClip( m_renderer,
                                                                m_dragItem->clip(),
                                                                m_dragItem->trackNumber(),
                                                                (qint64)mappedXPos,
                                                                MainWorkflow::VideoTrack ) );
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

void TracksView::setTool( ToolButtons button )
{
    m_tool = button;
    if ( m_tool == TOOL_CUT )
        scene()->clearSelection();
}

void TracksView::resizeEvent( QResizeEvent* event )
{
    QGraphicsView::resizeEvent( event );
}

void TracksView::drawBackground( QPainter* painter, const QRectF& rect )
{
    // Draw the tracks separators
    painter->setPen( QPen( QColor( 72, 72, 72 ) ) );
    for ( int i = 0; i < m_layout->count(); ++i )
    {
        QGraphicsItem* gi = m_layout->itemAt( i )->graphicsItem();
        if ( !gi ) continue;
        GraphicsTrack* track = qgraphicsitem_cast<GraphicsTrack*>( gi );
        if ( !track ) continue;

        QRectF trackRect = track->mapRectToScene( track->boundingRect() );
        if ( track->mediaType() == MainWorkflow::VideoTrack )
            painter->drawLine( trackRect.left(), trackRect.top(), rect.right(), trackRect.top() );
        else
            painter->drawLine( trackRect.left(), trackRect.bottom(), rect.right(), trackRect.bottom() );
    }

    // Audio/Video separator
    QRectF r = rect;
    r.setWidth( r.width() + 1 );

    painter->setWorldMatrixEnabled( false );

    QLinearGradient g( 0, m_separator->y(), 0, m_separator->y() + m_separator->boundingRect().height() );
    QColor base = palette().window().color();
    QColor end = palette().dark().color();
    g.setColorAt( 0, end );
    g.setColorAt( 0.1, base );
    g.setColorAt( 0.9, base );
    g.setColorAt( 1.0, end );

    painter->setBrush( QBrush( g ) );
    painter->setPen( Qt::transparent );
    painter->drawRect( 0,
                       (int) m_separator->y(),
                       (int) r.right(),
                       (int) m_separator->boundingRect().height() );
}

void TracksView::mouseMoveEvent( QMouseEvent* event )
{
    if ( event->modifiers() == Qt::NoModifier &&
         event->buttons() == Qt::LeftButton &&
         m_actionMove == true )
    {
        // The move action is obviously executed
        m_actionMoveExecuted = true;

        m_actionItem->setOpacity( 0.6 );
        if ( m_actionRelativeX < 0 )
            m_actionRelativeX = event->pos().x() - mapFromScene( m_actionItem->pos() ).x();
        moveMediaItem( m_actionItem, QPoint( event->pos().x() - m_actionRelativeX, event->pos().y() ) );
    }
    else if ( event->modifiers() == Qt::NoModifier &&
              event->buttons() == Qt::LeftButton &&
              m_actionResize == true )
    {
        QPointF itemPos = m_actionItem->mapToScene( 0, 0 );
        QPointF itemNewSize = mapToScene( event->pos() ) - itemPos;

        //FIXME: BEGIN UGLY
        GraphicsTrack* track = getTrack( m_actionItem->mediaType(), m_actionItem->trackNumber() );
        Q_ASSERT( track );

        QPointF collidePos = track->sceneBoundingRect().topRight();
        collidePos.setX( itemPos.x() + itemNewSize.x() );

        QList<QGraphicsItem*> gi = scene()->items( collidePos );

        bool collide = false;
        for ( int i = 0; i < gi.count(); ++i )
        {
            AbstractGraphicsMediaItem* mi = dynamic_cast<AbstractGraphicsMediaItem*>( gi.at( i ) );
            if ( mi && mi != m_actionItem )
            {
                collide = true;
                break;
            }
        }
        // END UGLY

        if ( !collide )
        {
            if ( m_actionResizeType == AbstractGraphicsMediaItem::END )
            {
                qint64 distance = mapToScene( event->pos() ).x() - m_actionResizeStart;
                qint64 newsize = qMax( m_actionResizeBase - distance, (qint64)0 );
                m_actionItem->resize( newsize , AbstractGraphicsMediaItem::END );
            }
            else
            {
                m_actionItem->resize( itemNewSize.x(), AbstractGraphicsMediaItem::BEGINNING );
            }
        }
    }

    QGraphicsView::mouseMoveEvent( event );
}

void TracksView::mousePressEvent( QMouseEvent* event )
{
    QList<AbstractGraphicsMediaItem*> mediaCollisionList = mediaItems( event->pos() );

    if ( event->modifiers() == Qt::ControlModifier && mediaCollisionList.count() == 0 )
    {
        setDragMode( QGraphicsView::ScrollHandDrag );
        event->accept();
    }
    else if ( event->modifiers() == Qt::NoModifier &&
         event->button() == Qt::LeftButton &&
         tool() == TOOL_DEFAULT &&
         mediaCollisionList.count() == 1 )
    {
        AbstractGraphicsMediaItem* item = mediaCollisionList.at( 0 );

        QPoint itemEndPos = mapFromScene( item->mapToScene( item->boundingRect().bottomRight() ) );
        QPoint itemPos = mapFromScene( item->mapToScene( 0, 0 ) );
        QPoint clickPos = event->pos() - itemPos;
        QPoint itemSize = itemEndPos - itemPos;

        if ( clickPos.x() < RESIZE_ZONE || clickPos.x() > ( itemSize.x() - RESIZE_ZONE ) )
        {
            if ( clickPos.x() < RESIZE_ZONE )
                m_actionResizeType = AbstractGraphicsMediaItem::END;
            else
                m_actionResizeType = AbstractGraphicsMediaItem::BEGINNING;
            m_actionResize = true;
            m_actionResizeStart = mapToScene( event->pos() ).x();
            m_actionResizeBase = item->clip()->getLength();
            m_actionResizeOldBegin = item->clip()->getBegin();
            m_actionItem = item;
        }
        else if ( item->moveable() )
        {
            m_actionMove = true;
            m_actionMoveExecuted = false;
            m_actionItem = item;
        }
        scene()->clearSelection();
        item->setSelected( true );
        event->accept();
    }
    else if ( event->modifiers() == Qt::ControlModifier &&
              event->button() == Qt::LeftButton &&
              tool() == TOOL_DEFAULT &&
              mediaCollisionList.count() == 1 )
    {
        AbstractGraphicsMediaItem* item = mediaCollisionList.at( 0 );
        item->setSelected( !item->isSelected() );
        event->accept();
    }
    else if ( event->modifiers() & Qt::ShiftModifier && mediaCollisionList.count() == 0 )
    {
        setDragMode( QGraphicsView::RubberBandDrag );
        if ( !event->modifiers() & Qt::ControlModifier )
            scene()->clearSelection();
        event->accept();
    }

    QGraphicsView::mousePressEvent( event );
}

void TracksView::mouseReleaseEvent( QMouseEvent* event )
{
    if ( m_actionMove && m_actionMoveExecuted )
    {
        m_actionItem->setOpacity( 1.0 );

        GraphicsMovieItem* movieItem = qgraphicsitem_cast<GraphicsMovieItem*>( m_actionItem );
        if ( movieItem )
        {
            updateDuration();
            if ( m_layout->itemAt( 0 )->graphicsItem()->childItems().count() > 0 )
                addVideoTrack();
            Commands::trigger( new Commands::MainWorkflow::MoveClip( m_mainWorkflow,
                                                                     movieItem->clip()->getUuid(),
                                                                     movieItem->oldTrackNumber,
                                                                     movieItem->trackNumber(),
                                                                     movieItem->startPos(),
                                                                     MainWorkflow::VideoTrack ) );
            movieItem->oldTrackNumber = movieItem->trackNumber();
            movieItem->oldPosition = movieItem->startPos();
            m_actionRelativeX = -1;
            m_actionItem = NULL;
        }
    }
    else if ( m_actionResize )
    {
        Clip* clip = m_actionItem->clip();
        //This is a "pointless action". The resize already occured. However, by doing this
        //we can have an undo action.
        Commands::trigger( new Commands::MainWorkflow::ResizeClip( m_renderer, clip->getUuid(), clip->getBegin(),
                                                                   clip->getEnd(), m_actionResizeOldBegin, m_actionResizeOldBegin + m_actionResizeBase,
                                                                   m_actionItem->pos().x(), m_actionResizeStart, m_actionItem->trackNumber(), MainWorkflow::VideoTrack ) );
        updateDuration();
    }

    m_actionMove = false;
    m_actionResize = false;

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

void TracksView::setCursorPos( qint64 pos )
{
    if ( pos < 0 ) pos = 0;
    m_cursorLine->frameChanged( pos, MainWorkflow::TimelineCursor );
}

qint64 TracksView::cursorPos()
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
        if ( ( item->startPos() + item->boundingRect().width() ) > projectDuration )
            projectDuration = ( int ) ( item->startPos() + item->boundingRect().width() );
    }

    m_projectDuration = projectDuration;

    // Make sure that the width is not below zero
    int minimumWidth = qMax( m_projectDuration, 0 );

    // PreferredWidth not working ?
    m_layout->setMinimumWidth( minimumWidth );
    m_layout->setMaximumWidth( minimumWidth );

    setSceneRect( m_layout->contentsRect() );

    emit durationChanged( m_projectDuration );
}

GraphicsTrack* TracksView::getTrack( MainWorkflow::TrackType type, unsigned int number )
{
    for (int i = 0; i < m_layout->count(); ++i )
    {
        QGraphicsItem* gi = m_layout->itemAt( i )->graphicsItem();
        GraphicsTrack* track = qgraphicsitem_cast<GraphicsTrack*>( gi );
        if ( !track ) continue;
        if ( track->mediaType() != type ) continue;
        if ( track->trackNumber() == number )
            return track;
    }
    return NULL;
}

void TracksView::split( GraphicsMovieItem* item, qint64 frame )
{
    //frame is the number of frame from the beginning of the clip
    //item->startPos() is the position of the splitted clip (in frame)
    //therefore, the position of the newly created clip is
    //the splitted clip pos + the splitting point (ie startPos() + frame)
    Commands::trigger( new Commands::MainWorkflow::SplitClip( m_renderer, item->clip(), item->trackNumber(),
                                                              item->startPos() + frame, frame + item->clip()->getBegin(),
                                                              MainWorkflow::VideoTrack ) );
}
