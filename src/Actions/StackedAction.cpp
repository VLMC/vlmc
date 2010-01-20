/*****************************************************************************
 * StackedAction.cpp: Stores an action to be executed later
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
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

#include "StackedAction.h"

using namespace Action;

Generic::Generic( Type type ) : m_type( type )
{
}

Generic::~Generic()
{
}

bool
Generic::isOpposite( const Generic* ) const
{
    return false;
}

Generic::Type
Generic::getType() const
{
    return m_type;
}

Workflow::Workflow( MainWorkflow *mainWorkflow, Type type )  :
        Generic( type ),
        m_mainWorkflow( mainWorkflow )
{
}

Track::Track( MainWorkflow *mainWorkflow, uint32_t trackId,
              MainWorkflow::TrackType trackType, Type type) :
        Workflow( mainWorkflow, type ),
        m_trackId( trackId ),
        m_trackType( trackType )
{
}

AddClip::AddClip(MainWorkflow *mainWorkflow, uint32_t trackId,
                 MainWorkflow::TrackType trackType, Clip *clip, qint64 startingPos ) :
        Track( mainWorkflow, trackId, trackType, Add ),
        m_clip( clip ), m_startingPos( startingPos )
{
}

void
AddClip::execute()
{
    m_mainWorkflow->addClip( m_clip, m_trackId, m_startingPos, m_trackType );
}

RemoveClip::RemoveClip( MainWorkflow* mainWorkflow, uint32_t trackId,
                        MainWorkflow::TrackType trackType, const QUuid& uuid ) :
    Track( mainWorkflow, trackId, trackType, Remove ),
    m_uuid( uuid )
{
}

void
RemoveClip::execute()
{
    m_mainWorkflow->removeClip( m_uuid, m_trackId, m_trackType );
}

MoveClip::MoveClip( MainWorkflow* mainWorkflow, const QUuid& uuid, uint32_t oldTrack,
                      uint32_t newTrack, qint64 newPos, MainWorkflow::TrackType trackType,
                      bool undoRedoAction ) :
                    Track( mainWorkflow, oldTrack, trackType, Move ),
                    m_uuid( uuid ), m_newTrack( newTrack ),
                    m_newPos( newPos ), m_undoRedoAction( undoRedoAction )
{
}

void
MoveClip::execute()
{
    m_mainWorkflow->moveClip( m_uuid, m_trackId, m_newTrack, m_newPos, m_trackType, m_undoRedoAction );
}

ResizeClip::ResizeClip( Clip* clip, qint64 newBegin, qint64 newEnd, bool resizeForSplit ) :
        Generic( Resize ),
        m_clip( clip ),
        m_newBegin( newBegin ),
        m_newEnd( newEnd ),
        m_resizeForSplit( resizeForSplit )
{
}

void
ResizeClip::execute()
{
    m_clip->setBoundaries( m_newBegin, m_newEnd, m_resizeForSplit );
}

Pause::Pause( MainWorkflow* mainWorkflow ) :
        Workflow( mainWorkflow, Generic::Pause )
{
}

void
Pause::execute()
{
    m_mainWorkflow->pause();
}

bool
Pause::isOpposite( const Generic* act ) const
{
    return ( act->getType() == Unpause );
}

Unpause::Unpause( MainWorkflow* mainWorkflow ) :
        Workflow( mainWorkflow, Generic::Unpause )
{
}

void
Unpause::execute()
{
    m_mainWorkflow->unpause();
}

bool
Unpause::isOpposite( const Generic* act ) const
{
    return ( act->getType() == Generic::Pause );
}
