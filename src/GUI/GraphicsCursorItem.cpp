/*****************************************************************************
 * GraphicsCursorItem.cpp: Timeline's cursor
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

#include <QtDebug>
#include "GraphicsCursorItem.h"

GraphicsCursorItem::GraphicsCursorItem( const QPen& pen ) :
        m_pen( pen )
{
    setFlags( QGraphicsItem::ItemIgnoresTransformations | QGraphicsItem::ItemIsMovable );
    setCursor( QCursor( Qt::SizeHorCursor ) );
    setZValue( 100 );

    m_boundingRect = QRectF( -2, 0, 3, 0 );
}

QRectF GraphicsCursorItem::boundingRect() const
{
    return m_boundingRect;
}

void GraphicsCursorItem::paint( QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* )
{
    painter->setPen( m_pen );
    painter->drawLine( 0, 0, 0, m_boundingRect.height() );
}

QVariant GraphicsCursorItem::itemChange( GraphicsItemChange change, const QVariant& value )
{
    //Position is changing :
    if ( change == ItemPositionChange )
    {
        qreal posX = value.toPointF().x();
        if ( posX < 0 ) posX = 0;
        return QPoint( ( int ) posX, ( int ) pos().y() );
    }
    //The position HAS changed, ie we released the slider, or setPos has been called.
    else if ( change == ItemPositionHasChanged )
    {
        emit cursorPositionChanged( ( qint64 ) pos().x() );
    }
    return QGraphicsItem::itemChange( change, value );
}

void GraphicsCursorItem::frameChanged( qint64 newFrame, MainWorkflow::FrameChangedReason reason )
{
    if ( reason != MainWorkflow::TimelineCursor )
    {
        setPos( newFrame, pos().y() );
    }
}

void GraphicsCursorItem::setHeight( int height )
{
    prepareGeometryChange();
    m_boundingRect.setHeight( height );
}
