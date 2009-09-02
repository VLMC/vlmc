/*****************************************************************************
 * Commands.h: Contains all the implementation of VLMC commands.
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

#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <QUndoCommand>
#include <QObject>
#include <QVector>
#include "UndoStack.h"
#include "MainWorkflow.h"
#include "Clip.h"

#define NEW_COMMAND(x)      class   x : public QUndoCommand

namespace Commands
{
    void        trigger( QUndoCommand* command )
    {
        UndoStack::getInstance()->push( command );
    }

    namespace   MainWorkflow
    {
        struct ClipActionInfo
        {
            Clip*   clip;
            quint32 trackNumber;
            qint64  pos;
        };

        NEW_COMMAND( AddClip )
        {
            public:
                AddClip( ::MainWorkflow* workflow, Clip* clip, unsigned int trackNumber, qint64 pos ) :
                        m_workflow( workflow ), m_clip( clip ), m_trackNumber( trackNumber ), m_pos( pos )
                {
                    setText( QObject::tr( "Adding clip to track %1" ).arg( QString::number( trackNumber ) ) );
                }
                virtual void    redo()
                {
                    m_workflow->addClip( m_clip, m_trackNumber, m_pos );
                }
                virtual void    undo()
                {
                    m_workflow->removeClip( m_clip->getTimelineUuid(), m_trackNumber );
                }
            private:
                ::MainWorkflow* m_workflow;
                Clip*           m_clip;
                unsigned int    m_trackNumber;
                qint64          m_pos;
        };

        NEW_COMMAND( MoveClip )
        {
            public:
                MoveClip( ::MainWorkflow* workflow, const QUuid& uuid,
                        unsigned int oldTrack, qint64 oldPos, unsigned int newTrack, qint64 newPos ) :
                    m_workflow( workflow ), m_uuid( uuid ), m_oldTrack( oldTrack ),
                    m_newTrack( newTrack ), m_pos( newPos ), m_oldPos( oldPos )
                {
                    setText( QObject::tr( "Moving clip" ) );
                    m_undoRedoAction = false;
                }
                virtual void    redo()
                {
                    qDebug() << "Moving from track" << m_oldTrack << "to" << m_newTrack << "at pos" << m_pos;
                    m_workflow->moveClip( m_uuid, m_oldTrack, m_newTrack, m_pos, m_undoRedoAction );
                    m_undoRedoAction = true;
                }
                virtual void    undo()
                {
                    qDebug() << "Moving from track" << m_newTrack << "to" << m_oldTrack << "at pos" << m_oldPos;
                    m_workflow->moveClip( m_uuid, m_newTrack, m_oldTrack, m_oldPos, m_undoRedoAction );
                    m_undoRedoAction = true;
                }

            private:
                ::MainWorkflow*     m_workflow;
                QUuid               m_uuid;
                unsigned int        m_oldTrack;
                unsigned int        m_newTrack;
                qint64              m_pos;
                qint64              m_oldPos;
                bool                m_undoRedoAction;
        };

        NEW_COMMAND( RemoveClips )
        {
            public:
                RemoveClips( ::MainWorkflow* workflow, const QVector<ClipActionInfo>& clipsInfos ) :
                        m_workflow( workflow ), m_clips( clipsInfos )
                {
                    setText( QObject::tr( "Remove clip" ) );
                }
                virtual void redo()
                {
                    for (int i = 0; i < m_clips.size(); ++i )
                        m_workflow->removeClip( m_clips.at( i ).clip->getTimelineUuid(), m_clips.at( i ).trackNumber );
                }
                virtual void undo()
                {
                    for (int i = 0; i < m_clips.size(); ++i )
                        m_workflow->addClip( m_clips.at( i ).clip, m_clips.at( i ).trackNumber, m_clips.at( i ).pos );
                }

            private:
                ::MainWorkflow*             m_workflow;
                QVector<ClipActionInfo>     m_clips;
        };
    }
}

#endif // COMMANDS_HPP
