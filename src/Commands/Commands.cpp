/*****************************************************************************
 * Commands.cpp: Contains all the implementation of VLMC commands.
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
 *          Ludovic Fauvet <etix@l0cal.com>
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

#include <QUndoCommand>
#include "Commands.h"
#include "UndoStack.h"
#include "MainWorkflow.h"
#include "Clip.h"

void Commands::trigger( QUndoCommand* command )
{
    UndoStack::getInstance()->push( command );
}

Commands::MainWorkflow::AddClip::AddClip( WorkflowRenderer* renderer, Clip* clip,
                                          unsigned int trackNumber, qint64 pos,
                                          ::MainWorkflow::TrackType trackType ) :
        m_renderer( renderer ),
        m_clip( clip ),
        m_trackNumber( trackNumber ),
        m_pos( pos ),
        m_trackType( trackType )
{
    setText( QObject::tr( "Adding clip to track %1" ).arg( QString::number( trackNumber ) ) );
}

Commands::MainWorkflow::AddClip::~AddClip()
{
    delete m_clip;
}

void Commands::MainWorkflow::AddClip::redo()
{
    m_renderer->addClip( m_clip, m_trackNumber, m_pos, m_trackType );
}

void Commands::MainWorkflow::AddClip::undo()
{
    m_renderer->removeClip( m_clip->getUuid(), m_trackNumber, m_trackType );
}

Commands::MainWorkflow::MoveClip::MoveClip( ::MainWorkflow* workflow, const QUuid& uuid,
          unsigned int oldTrack, qint64 oldPos, unsigned int newTrack, qint64 newPos,
          ::MainWorkflow::TrackType trackType ) :
    m_workflow( workflow ), m_uuid( uuid ), m_oldTrack( oldTrack ),
    m_newTrack( newTrack ), m_pos( newPos ), m_oldPos( oldPos ),
    m_trackType( trackType )
{
    setText( QObject::tr( "Moving clip" ) );
    m_undoRedoAction = false;
}

void Commands::MainWorkflow::MoveClip::redo()
{
    m_workflow->moveClip( m_uuid, m_oldTrack, m_newTrack, m_pos, m_trackType, m_undoRedoAction );
    m_undoRedoAction = true;
}

void Commands::MainWorkflow::MoveClip::undo()
{
    m_workflow->moveClip( m_uuid, m_newTrack, m_oldTrack, m_oldPos, m_trackType, m_undoRedoAction );
    m_undoRedoAction = true;
}

Commands::MainWorkflow::RemoveClips::RemoveClips( WorkflowRenderer* renderer, const QVector<ClipActionInfo>& clipsInfos ) :
        m_renderer( renderer ), m_clips( clipsInfos )
{
    setText( QObject::tr( "Remove clip" ) );
}

Commands::MainWorkflow::RemoveClips::~RemoveClips()
{
    int size = m_clips.size();
    for (int i = 0; i < size; ++i )
        delete m_clips[i].clip;
}

void Commands::MainWorkflow::RemoveClips::redo()
{
    for (int i = 0; i < m_clips.size(); ++i )
    {
        const ClipActionInfo&   clipInfo = m_clips.at( i );
        m_renderer->removeClip( clipInfo.clip->getUuid(), clipInfo.trackNumber, clipInfo.trackType );
    }
}
void Commands::MainWorkflow::RemoveClips::undo()
{
    for (int i = 0; i < m_clips.size(); ++i )
        m_renderer->addClip( m_clips.at( i ).clip, m_clips.at( i ).trackNumber, m_clips.at( i ).pos, m_clips.at( i ).trackType );
}

Commands::MainWorkflow::ResizeClip::ResizeClip( ::MainWorkflow* mainWorkflow, const QUuid& uuid, unsigned int trackId,
            qint64 newBegin, qint64 newEnd, ::MainWorkflow::TrackType trackType ) :
    m_mainWorkflow( mainWorkflow ),
    m_newBegin( newBegin ),
    m_newEnd( newEnd ),
    m_trackType( trackType )
{
    m_clip = mainWorkflow->getClip( uuid, trackId, m_trackType );
    m_oldBegin = m_clip->getBegin();
    m_oldEnd = m_clip->getEnd();
}

void Commands::MainWorkflow::ResizeClip::redo()
{
    m_clip->setBegin( m_newBegin );
    m_clip->setEnd( m_newEnd );
}

void Commands::MainWorkflow::ResizeClip::undo()
{
    m_clip->setBegin( m_oldBegin );
    m_clip->setEnd( m_oldEnd );
}

Commands::MainWorkflow::SplitClip::SplitClip( WorkflowRenderer* renderer, Clip* toSplit, uint32_t trackId,
                           qint64 newClipPos, qint64 newClipBegin, ::MainWorkflow::TrackType trackType ) :
    m_renderer( renderer ),
    m_toSplit( toSplit ),
    m_newClip( NULL ),
    m_trackId( trackId ),
    m_newClipPos( newClipPos ),
    m_newClipBegin( newClipBegin ),
    m_clipOldEnd( toSplit->getEnd() ),
    m_trackType( trackType )
{
    setText( QObject::tr("Splitting clip") );
}

void    Commands::MainWorkflow::SplitClip::redo()
{
    Q_ASSERT( m_newClip == NULL );

    m_clipOldEnd = m_toSplit->getEnd();
    m_newClip = m_renderer->split( m_toSplit, m_trackId, m_newClipPos, m_newClipBegin, m_trackType );
}

void    Commands::MainWorkflow::SplitClip::undo()
{
    m_renderer->unsplit( m_toSplit, m_newClip, m_trackId, m_clipOldEnd, m_trackType );
    m_newClip = NULL;
}
