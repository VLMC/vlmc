/*****************************************************************************
 * GraphicsTrack.cpp: Graphically represent a track in the timeline
 *****************************************************************************
 * Copyright (C) 2008-2010 the VLMC team
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

#include "TracksView.h"
#include "GraphicsTrack.h"

GraphicsTrack::GraphicsTrack( MainWorkflow::TrackType type, quint32 trackNumber,
                              QGraphicsItem *parent ) : QGraphicsWidget( parent )
{
    m_type = type;
    m_trackNumber = trackNumber;
    m_enabled = true;

    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    setContentsMargins( 0, 0, 0, 0 );
    setZValue( 1 );
}

void
GraphicsTrack::setHeight( int height )
{
    setPreferredHeight( height );
    adjustSize();
    updateGeometry();
}

int
GraphicsTrack::height()
{
    return preferredHeight();
}

void
GraphicsTrack::setTrackEnabled( bool enabled )
{
    if( enabled == m_enabled )
        return;
    m_enabled = enabled;

    if( enabled )
        MainWorkflow::getInstance()->unmuteTrack( m_trackNumber, m_type );
    else
        MainWorkflow::getInstance()->muteTrack( m_trackNumber, m_type );
}

bool
GraphicsTrack::isEnabled()
{
    return m_enabled;
}

quint32
GraphicsTrack::trackNumber()
{
    return m_trackNumber;
}

MainWorkflow::TrackType
GraphicsTrack::mediaType()
{
    return m_type;
}
