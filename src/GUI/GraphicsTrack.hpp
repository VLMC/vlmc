/*****************************************************************************
 * GraphicsTrack.hpp: Graphically represent a track in the timeline
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

#ifndef GRAPHICSTRACK_HPP
#define GRAPHICSTRACK_HPP

#include <QGraphicsWidget>
#include <QPainter>

class GraphicsTrack : public QGraphicsWidget
{
    Q_OBJECT

public:
    enum { Type = UserType + 2 };
    enum MediaType
    {
        Video,
        Audio
    };
    GraphicsTrack( MediaType type, quint32 trackNumber, QGraphicsItem* parent = 0 ) : QGraphicsWidget( parent )
    {
        m_type = type;
        m_trackNumber = trackNumber;

        setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
        setContentsMargins( 0, 0, 0, 0 );
        setZValue( 1 );
    }
    quint32 trackNumber()
    {
        return m_trackNumber;
    }
    virtual int type() const { return Type; }

protected:
    virtual void paint( QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* = 0 )
    {
        Q_UNUSED( painter );
        /*
        Debugging of tracks.
        Do not remove this block.

        if ( m_type == Video )
            painter->setBrush( Qt::green );
        else
            painter->setBrush( Qt::blue );
        painter->setPen( Qt::transparent );
        painter->drawRect( rect() );
        */
    }

private:
    MediaType m_type;
    quint32 m_trackNumber;
};

#endif // GRAPHICSTRACK_HPP
