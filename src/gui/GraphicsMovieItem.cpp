/*****************************************************************************
 * GraphicsMovieItem.cpp: Represent a movie graphically in the timeline
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

#include "GraphicsMovieItem.h"
#include "TracksView.h"

GraphicsMovieItem::GraphicsMovieItem( Media* media ) : m_media ( media ), m_width( 0 ), m_height( 0 )
{

}

GraphicsMovieItem::~GraphicsMovieItem()
{
}

QRectF GraphicsMovieItem::boundingRect() const
{
    return QRectF( 0, 0, m_width, m_height );
}

void GraphicsMovieItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    painter->setBrush( Qt::red );
    painter->drawRect( boundingRect() );
}


void GraphicsMovieItem::setWidth( int width )
{
    m_width = width;
}

void GraphicsMovieItem::setHeight( int height )
{
    m_height = height;
}
