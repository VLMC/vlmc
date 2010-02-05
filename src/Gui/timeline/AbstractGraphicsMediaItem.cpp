/*****************************************************************************
 * AbstractGraphicsMediaItem.h: Base class for media representation
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
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

#include <QMenu>
#include <QColorDialog>
#include "AbstractGraphicsMediaItem.h"
#include "TracksView.h"
#include "TracksScene.h"
#include "GraphicsTrack.h"

#include "Clip.h"
#include "Commands.h"

AbstractGraphicsMediaItem::AbstractGraphicsMediaItem() :
        oldTrackNumber( -1 ), oldPosition( -1 ), m_tracksView( NULL ),
        m_group( NULL ), m_width( 0 ), m_height( 0 ), m_resizeExpected( false ),
        m_muted( false )
{

}

AbstractGraphicsMediaItem::~AbstractGraphicsMediaItem()
{
    ungroup();
}

TracksScene* AbstractGraphicsMediaItem::scene()
{
    return qobject_cast<TracksScene*>( QGraphicsItem::scene() );
}

TracksView* AbstractGraphicsMediaItem::tracksView()
{
    return m_tracksView;
}

QRectF AbstractGraphicsMediaItem::boundingRect() const
{
    return QRectF( 0, 0, (qreal)m_width, (qreal)m_height );
}

void AbstractGraphicsMediaItem::setWidth( qint64 width )
{
    prepareGeometryChange();
    m_width = width;
}

void AbstractGraphicsMediaItem::setHeight( qint64 height )
{
    prepareGeometryChange();
    m_height = height;
}

quint32 AbstractGraphicsMediaItem::trackNumber()
{
    if ( parentItem() )
    {
        GraphicsTrack* graphicsTrack = qgraphicsitem_cast<GraphicsTrack*>( parentItem() );
        if ( graphicsTrack )
            return graphicsTrack->trackNumber();
    }
    return -1;
}

void AbstractGraphicsMediaItem::group( AbstractGraphicsMediaItem* item )
{
    Q_ASSERT( item );
    if ( m_group )
        ungroup();
    item->m_group = this;
    m_group = item;
}

void AbstractGraphicsMediaItem::ungroup()
{
    if ( !m_group ) return;
    m_group->m_group = NULL;
    m_group = NULL;
}

AbstractGraphicsMediaItem* AbstractGraphicsMediaItem::groupItem()
{
    return m_group;
}

void AbstractGraphicsMediaItem::contextMenuEvent( QGraphicsSceneContextMenuEvent* event )
{
    if ( !tracksView() )
        return;

    QMenu menu( tracksView() );

    QAction* removeAction = menu.addAction( "Remove" );
    QAction* muteAction = menu.addAction( "Mute" );
    muteAction->setCheckable( true );
    muteAction->setChecked( m_muted );

    QAction* linkAction = NULL;
    QAction* unlinkAction = NULL;

    if ( groupItem() )
        unlinkAction = menu.addAction( "Unlink" );
    else
    {
        QList<QGraphicsItem*> items = scene()->selectedItems();
        linkAction = menu.addAction( "Link" );

        if ( items.count() != 2 )
            linkAction->setEnabled( false );
    }

    menu.addSeparator();

    QAction* changeColorAction = menu.addAction( "Set color" );

    QAction* selectedAction = menu.exec( event->screenPos() );

    if ( !selectedAction )
        return;

    if ( selectedAction == removeAction )
        scene()->askRemoveSelectedItems();
    else if ( selectedAction == muteAction )
    {

        if ( ( m_muted = muteAction->isChecked() ) )
        {
            tracksView()->m_mainWorkflow->muteClip( clip()->getUuid(),
                                                    trackNumber(),
                                                    mediaType() );
        }
        else
        {
            tracksView()->m_mainWorkflow->unmuteClip( clip()->getUuid(),
                                                    trackNumber(),
                                                    mediaType() );
        }
    }
    else if ( selectedAction == linkAction )
    {
        QList<QGraphicsItem*> items = scene()->selectedItems();

        AbstractGraphicsMediaItem* item1;
        AbstractGraphicsMediaItem* item2;

        item1 = dynamic_cast<AbstractGraphicsMediaItem*>( items.at( 0 ) );
        item2 = dynamic_cast<AbstractGraphicsMediaItem*>( items.at( 1 ) );

        Q_ASSERT( item1 );
        Q_ASSERT( item2 );

        if ( item1->mediaType() != item2->mediaType() )
        {
            item1->group( item2 );
            tracksView()->moveMediaItem( item1, item1->trackNumber(), item1->startPos() );
        }
    }
    else if ( selectedAction == unlinkAction )
    {
        QList<QGraphicsItem*> items = scene()->selectedItems();

        AbstractGraphicsMediaItem* item;
        item = dynamic_cast<AbstractGraphicsMediaItem*>( items.at( 0 ) );

        item->ungroup();
    }
    else if ( selectedAction == changeColorAction )
    {
        m_itemColor = QColorDialog::getColor( m_itemColor, tracksView() );
        update();
    }

}

void AbstractGraphicsMediaItem::setStartPos( qint64 position )
{
    QGraphicsItem::setPos( (qreal)position, 0 );
}

qint64 AbstractGraphicsMediaItem::startPos()
{
    return qRound64( QGraphicsItem::pos().x() );
}

void AbstractGraphicsMediaItem::resize( qint64 size, From from )
{
    Q_ASSERT( clip() );

    if ( size < 1 )
        return;

    if ( clip()->getParent()->getFileType() != Media::Image )
        if ( size > clip()->getMaxEnd() )
            return;

    if ( from == BEGINNING )
    {
        if ( clip()->getParent()->getFileType() != Media::Image )
            if ( clip()->getBegin() + size > clip()->getMaxEnd() )
                return;
        //FIXME
//        tracksView()->getRenderer()->resizeClip( clip(), clip()->getBegin(), clip()->getBegin() + size, 0, //This parameter is unused in this case
//                                                 trackNumber(), mediaType() );
    }
    else
    {
        if ( clip()->getParent()->getFileType() != Media::Image )
        {
            qint64 newBegin = qMax( clip()->getEnd() - size, (qint64)0 );
            if ( clip()->getMaxBegin() > newBegin )
                return;

            m_resizeExpected = true;
            qint64 oldLength = clip()->getLength();
            qint64  newStart = startPos() + ( oldLength - size );
            if ( newStart < 0 )
                return ;
//            tracksView()->getRenderer()->resizeClip( clip(), qMax( clip()->getEnd() - size, (qint64)0 ), clip()->getEnd(),
//                                                     newStart, trackNumber(), mediaType() );
//            setStartPos( newStart );
            //FIXME
        }
        else
        {
            m_resizeExpected = true;
            qint64 oldLength = clip()->getLength();
//            Commands::trigger( new Commands::MainWorkflow::ResizeClip( ))
//            tracksView()->getRenderer()->resizeClip( clip(), 0, size, startPos() + ( oldLength - size ),
//                                                     trackNumber(), mediaType() );
//            setStartPos( startPos() + ( oldLength - size ) );
            //FIXME
        }
    }

    setWidth( clip()->getLength() );
}

void AbstractGraphicsMediaItem::adjustLength()
{
    if ( m_resizeExpected == true )
    {
        m_resizeExpected = false;
        return ;
    }
    Q_ASSERT( clip() );
    resize( clip()->getLength() );
}

bool AbstractGraphicsMediaItem::resizeZone( const QPointF& position )
{
    // Get the current transformation of the view and invert it.
    QTransform transform = tracksView()->transform().inverted();
    // Map the RESIZE_ZONE distance from the view to the item coordinates.
    QLine line = transform.map( QLine( 0, 0, RESIZE_ZONE, 0 ) );

    if ( position.x() < line.x2() ||
         position.x() > ( boundingRect().width() - line.x2() ) )
    {
        return true;
    }
    return false;
}

QColor AbstractGraphicsMediaItem::itemColor()
{
    return m_itemColor;
}
