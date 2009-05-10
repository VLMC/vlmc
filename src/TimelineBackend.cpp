/*****************************************************************************
 * TimelineBackend.cpp: Backend for Timeline
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Christophe Courtaut <christophe.courtaut@gmail.com>
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

#include "TimelineBackend.h"

TimelineBackend::TimelineBackend()
{
    m_audioTracks = new QMap<int, Track*>();
    m_videoTracks = new QMap<int, Track*>();
}

Track*  TimelineBackend::getTrack( TimelineBackend::TrackType type, int index ) const
{
    if ( type == TimelineBackend::Audio )
        return m_audioTracks->value( index, NULL );
    else if ( type == TimelineBackend::Video)
        return m_videoTracks->value( index, NULL );
    return NULL;
}

int  TimelineBackend::createNewTrack( TimelineBackend::TrackType type )
{
    Track* track = new Track();
    if ( type == TimelineBackend::Audio )
        m_audioTracks->insert( findNewIndex( type ), track );
    else
        m_videoTracks->insert( findNewIndex( type ), track );
    return getTrackIndex( type, track );
}

bool    TimelineBackend::removeTrack( TimelineBackend::TrackType type, int index )
{
    QMap<int, Track*>* map = ( type == TimelineBackend::Audio ) ? m_audioTracks : m_videoTracks;
    return map->remove( index );
}

int     TimelineBackend::getTrackIndex( TimelineBackend::TrackType type, Track* track ) const
{
    QMap<int, Track*>* map = ( type == TimelineBackend::Audio ) ? m_audioTracks : m_videoTracks;
    return map->key( track, -1 );
}

int     TimelineBackend::findNewIndex( TimelineBackend::TrackType type ) const
{
    QMap<int, Track*>* map = ( type == TimelineBackend::Audio ) ? m_audioTracks : m_videoTracks;
    int index = 0;
    while ( index < map->size() )
    {
        if ( !map->contains( index ) )
            return index;
        index++;
    }
    return index;
}

