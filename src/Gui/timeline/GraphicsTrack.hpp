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
#include <QDebug>
#include "MainWorkflow.h"

class GraphicsTrack : public QGraphicsWidget
{
    Q_OBJECT

public:
    enum { Type = UserType + 2 };

    GraphicsTrack( MainWorkflow::TrackType type, quint32 trackNumber, QGraphicsItem* parent = 0 ) : QGraphicsWidget( parent )
    {
        m_type = type;
        m_trackNumber = trackNumber;
        m_enabled = true;

        setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
        setContentsMargins( 0, 0, 0, 0 );
        setZValue( 1 );
    }

    void setHeight( int height )
    {
        setPreferredHeight( height );
        adjustSize();
        updateGeometry();
    }

    int height()
    {
        return preferredHeight();
    }

    void setTrackEnabled( bool enabled )
    {
        if ( enabled == m_enabled ) return;
        m_enabled = enabled;

        MainWorkflow::TrackType type;
        if ( m_type == MainWorkflow::AudioTrack )
            //TODO need audio support
            //type = MainWorkflow::AudioTrack;
            return;
        else
            type = MainWorkflow::VideoTrack;

        if ( enabled )
            MainWorkflow::getInstance()->unmuteTrack( m_trackNumber, type );
        else
            MainWorkflow::getInstance()->muteTrack( m_trackNumber, type );
    }

    bool trackEnabled()
    {
        return m_enabled;
    }

    quint32 trackNumber()
    {
        return m_trackNumber;
    }

    MainWorkflow::TrackType mediaType()
    {
        return m_type;
    }

    virtual int type() const { return Type; }

private:
    MainWorkflow::TrackType m_type;
    quint32 m_trackNumber;
    bool m_enabled;
};

#endif // GRAPHICSTRACK_HPP
