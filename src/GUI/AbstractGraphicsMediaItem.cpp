/*****************************************************************************
 * AbstractGraphicsMediaItem.h: Base class for media representation
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

#include "AbstractGraphicsMediaItem.h"
#include "TracksView.h"

#include "Clip.h"
#include "Commands.h"

AbstractGraphicsMediaItem::AbstractGraphicsMediaItem() :
        oldTrackNumber( -1 ), oldPosition( -1 ), m_tracksView( NULL ),
        m_group( NULL ), m_width( 0 ), m_height( 0 ), m_resizeExpected( false )
{

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

    if ( size > clip()->getMaxEnd() || size < 1 )
        return;

    if ( from == BEGINNING )
    {
        if ( clip()->getBegin() + size > clip()->getMaxEnd() )
            return ;
        tracksView()->getRenderer()->resizeClip( clip(), clip()->getBegin(), clip()->getBegin() + size, 0, //This parameter is unused in this case
                                                 trackNumber(), MainWorkflow::VideoTrack );
    }
    else
    {
        qint64 newBegin = qMax( clip()->getEnd() - size, (qint64)0 );
        if ( clip()->getMaxBegin() > newBegin )
            return ;
        qint64 oldLength = clip()->getLength();
//        if ( ( clip()->getEnd() - size ) < 0 )
//        {
//            qWarning( "Warning: resizing a region with a size below 0" );
//            size += clip()->getEnd() - size;
//        }
        m_resizeExpected = true;
        tracksView()->getRenderer()->resizeClip( clip(), qMax( clip()->getEnd() - size, (qint64)0 ), clip()->getEnd(),
                                                 startPos() + ( oldLength - size ), trackNumber(), MainWorkflow::VideoTrack );
        setStartPos( startPos() + ( oldLength - size ) );
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
