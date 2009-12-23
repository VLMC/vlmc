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
          unsigned int oldTrack, unsigned int newTrack, qint64 newPos,
          ::MainWorkflow::TrackType trackType ) :
    m_workflow( workflow ), m_uuid( uuid ), m_oldTrack( oldTrack ),
    m_newTrack( newTrack ), m_pos( newPos ), m_trackType( trackType )
{
    if ( oldTrack != newTrack )
        setText( QObject::tr( "Moving clip from track %1 to %2" ).arg(
                QString::number( oldTrack ), QString::number( newTrack ) ) );
    else
        setText( QObject::tr( "Moving clip" ) );
    m_undoRedoAction = false;
    m_oldPos = m_workflow->getClipPosition( uuid, oldTrack, trackType );
}

void Commands::MainWorkflow::MoveClip::redo()
{
//    qDebug() << "Moving clip from track" << m_oldTrack << "to" << m_newTrack << "at position:" << m_pos;
    m_workflow->moveClip( m_uuid, m_oldTrack, m_newTrack, m_pos, m_trackType, m_undoRedoAction );
    m_undoRedoAction = true;
}

void Commands::MainWorkflow::MoveClip::undo()
{
//    qDebug() << "Undo moving clip from track" << m_newTrack << "to" << m_oldTrack << "at position:" << m_oldPos;
    m_workflow->moveClip( m_uuid, m_newTrack, m_oldTrack, m_oldPos, m_trackType, m_undoRedoAction );
    m_undoRedoAction = true;
}

Commands::MainWorkflow::RemoveClips::RemoveClips( WorkflowRenderer* renderer, const QVector<ClipActionInfo>& clipsInfos ) :
        m_renderer( renderer ), m_clips( clipsInfos )
{
    setText( QObject::tr( "Remove clip" ) );
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

Commands::MainWorkflow::ResizeClip::ResizeClip( WorkflowRenderer* renderer, const QUuid& uuid,
                                                qint64 newBegin, qint64 newEnd,
                                                qint64 oldBegin, qint64 oldEnd,
                                                qint64 newPos, qint64 oldPos,
                                                uint32_t trackId,
                                                ::MainWorkflow::TrackType trackType ) :
    m_renderer( renderer ),
    m_uuid( uuid ),
    m_newBegin( newBegin ),
    m_newEnd( newEnd ),
    m_oldBegin( oldBegin ),
    m_oldEnd( oldEnd ),
    m_newPos( newPos ),
    m_oldPos( oldPos ),
    m_trackId( trackId ),
    m_trackType( trackType ),
    m_undoRedoAction( false )
{
    m_clip = ::MainWorkflow::getInstance()->getClip( uuid, trackId, m_trackType );
    setText( QObject::tr( "Resizing clip" ) );
}

void Commands::MainWorkflow::ResizeClip::redo()
{
    m_renderer->resizeClip( m_clip, m_newBegin, m_newEnd, m_newPos, m_trackId, m_trackType, m_undoRedoAction );
    m_undoRedoAction = true;
}

void Commands::MainWorkflow::ResizeClip::undo()
{
    //This code is complete crap.
    // We need to case, because when we redo a "begin-resize", we need to first resize, then move.
    //In the other cases, we need to move, then resize.
    if ( m_oldBegin == m_newBegin )
    {
        m_renderer->resizeClip( m_clip, m_oldBegin, m_oldEnd, m_oldPos, m_trackId, m_trackType, m_undoRedoAction );
    }
    else
    {
        m_clip->setBoundaries( m_oldBegin, m_oldEnd );
        ::MainWorkflow::getInstance()->moveClip( m_clip->getUuid(), m_trackId, m_trackId, m_oldPos, m_trackType, m_undoRedoAction );
    }
}

Commands::MainWorkflow::SplitClip::SplitClip( WorkflowRenderer* renderer, Clip* toSplit, uint32_t trackId,
                           qint64 newClipPos, qint64 newClipBegin, ::MainWorkflow::TrackType trackType ) :
    m_renderer( renderer ),
    m_toSplit( toSplit ),
    m_newClip( NULL ),
    m_trackId( trackId ),
    m_newClipPos( newClipPos ),
    m_newClipBegin( newClipBegin ),
    m_trackType( trackType )
{
    setText( QObject::tr("Splitting clip") );
}

Commands::MainWorkflow::SplitClip::~SplitClip()
{
    if ( m_newClip != NULL )
        delete m_newClip;
}

void    Commands::MainWorkflow::SplitClip::redo()
{
    m_newClip = m_renderer->split( m_toSplit, m_newClip, m_trackId, m_newClipPos, m_newClipBegin, m_trackType );
}

void    Commands::MainWorkflow::SplitClip::undo()
{
    m_renderer->unsplit( m_toSplit, m_newClip, m_trackId, m_trackType );
}
