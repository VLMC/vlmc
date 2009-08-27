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

#include <QPainter>
#include <QLinearGradient>
#include <QDebug>
#include <QTime>
#include "GraphicsMovieItem.h"
#include "TracksView.h"
#include "Timeline.h"

GraphicsMovieItem::GraphicsMovieItem( Clip* clip ) : m_clip( clip ), m_width( 0 ), m_height( 0 )
{
    setFlags( QGraphicsItem::ItemIsSelectable );

    QTime length = QTime().addMSecs( clip->getParent()->getLength() );
    QString tooltip( tr( "<p style='white-space:pre'><b>Name:</b> %1"
                     "<br><b>Length:</b> %2" )
                     .arg( clip->getParent()->getFileInfo()->fileName() )
                     .arg( length.toString("hh:mm:ss.zzz") ) );
    setToolTip( tooltip );

    m_movieTitle = new QGraphicsTextItem( this, scene() );
    m_movieTitle->setFlag( QGraphicsItem::ItemIgnoresTransformations );
    QFont titleFont = m_movieTitle->font();
    titleFont.setPointSize( 8 );
    m_movieTitle->setFont( titleFont );

    updateTitle();
}

GraphicsMovieItem::~GraphicsMovieItem()
{
}

QRectF GraphicsMovieItem::boundingRect() const
{
    return QRectF( 0, 0, m_width, m_height );
}

void GraphicsMovieItem::paint( QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* )
{
    QLinearGradient gradient( 0, 0, m_width, m_height );
    gradient.setColorAt( 0, QColor::fromRgb( 32, 52, 142  ) );
    gradient.setColorAt( 1, QColor::fromRgb( 43, 69, 181  ) );

    if ( isSelected() )
        painter->setPen( QPen( Qt::yellow ) );
    else
        painter->setPen( QPen( Qt::NoPen ) );

    painter->setBrush( QBrush( gradient ) );
    painter->drawRect( boundingRect() );

    updateTitle();
}

void GraphicsMovieItem::setWidth( int width )
{
    m_width = width;
}

void GraphicsMovieItem::setHeight( int height )
{
    m_height = height;
}

Clip* GraphicsMovieItem::clip() const
{
    return m_clip;
}

void GraphicsMovieItem::updateTitle()
{
    QFontMetrics fm( m_movieTitle->font() );
    QString text = m_clip->getParent()->getFileInfo()->fileName();

    static int lastWidth = 0;
    int width = Timeline::getInstance()->tracksView()->mapFromScene( boundingRect() ).boundingRect().width();
    if ( lastWidth == width ) return;

    lastWidth = width;
    //FIXME there is a small visual refresh bug here
    m_movieTitle->setPlainText( fm.elidedText( text, Qt::ElideRight, width ) );
}
