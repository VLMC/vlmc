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
#include "GraphicsMovieItem.h"
#include "TracksView.h"

GraphicsMovieItem::GraphicsMovieItem( Media* media ) : m_media ( media ), m_width( 0 ), m_height( 0 )
{
    setToolTip( media->getFileInfo()->fileName() );
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

    painter->setPen( QPen( Qt::NoPen ) );
    painter->setBrush( QBrush( gradient ) );
    painter->drawRect( boundingRect() );

    paintAudioSpectrum( painter );
}


void GraphicsMovieItem::setWidth( int width )
{
    m_width = width;
}

void GraphicsMovieItem::setHeight( int height )
{
    m_height = height;
}

void GraphicsMovieItem::paintAudioSpectrum( QPainter* painter )
{
    qreal x1, x2, y1, y2;
    QRectF tmp = boundingRect();
    tmp.getCoords( &x1, &y1, &x2, &y2 );

    painter->setPen( Qt::black );
    painter->setBrush( Qt::black );

    QLineF line;

    for (int i = 0; i < m_media->getAudioFrameList().size(); i++)
    {
        //qDebug() << "Frame: " << i << "/" << m_media->getAudioFrameList().size();
        for (unsigned int u = 0; u < m_media->getAudioNbSample(); u += 400)
        {


            int value = m_media->getAudioFrameList()[i][u];
            value /= 30;
            if( value > 48 ) value = 48;
            if( value < 0 ) value = 0;

            //qDebug() << "frame nb:" << i << " buff value: " << value;

            static QPointF point = QPointF( x1, ( y2 - y1 ) / 2 );

            qreal y = ( ( y2 - y1 ) / 2 ) - value / 2;
            x1 += 2;
            QPointF point2( x1, y );

            line.setPoints( point, point2);
            //line.setLine( x1 , y, x1 + 2, y);


            painter->drawLine(line);
            //painter->drawRect(tmp);

            point.setX( point2.x() );
            point.setY( point2.y() );
        }
    }
}

Media* GraphicsMovieItem::media() const
{
    return m_media;
}
