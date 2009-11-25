/*****************************************************************************
 * Commands.h: Contains all the implementation of VLMC commands.
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

#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include <QObject>
#include <QVector>
#include "UndoStack.h"
#include "MainWorkflow.h"
#include "Clip.h"
#include "WorkflowRenderer.h"

#define NEW_COMMAND(x)      class   x : public QUndoCommand

namespace Commands
{
    void        trigger( QUndoCommand* command );

    namespace   MainWorkflow
    {
        struct ClipActionInfo
        {
            Clip*                       clip;
            quint32                     trackNumber;
            qint64                      pos;
            ::MainWorkflow::TrackType   trackType;
        };

        NEW_COMMAND( AddClip )
        {
            public:
                AddClip( WorkflowRenderer* renderer, Clip* clip, unsigned int trackNumber, qint64 pos, ::MainWorkflow::TrackType trackType );
                virtual ~AddClip();
                virtual void    redo();
                virtual void    undo();
            private:
                WorkflowRenderer*           m_renderer;
                Clip*                       m_clip;
                unsigned int                m_trackNumber;
                qint64                      m_pos;
                ::MainWorkflow::TrackType   m_trackType;
        };

        NEW_COMMAND( MoveClip )
        {
            public:
                MoveClip( ::MainWorkflow* workflow, const QUuid& uuid,
                        unsigned int oldTrack, qint64 oldPos, unsigned int newTrack,
                        qint64 newPos, ::MainWorkflow::TrackType trackType );
                virtual void    redo();
                virtual void    undo();

            private:
                ::MainWorkflow*             m_workflow;
                QUuid                       m_uuid;
                unsigned int                m_oldTrack;
                unsigned int                m_newTrack;
                qint64                      m_pos;
                qint64                      m_oldPos;
                bool                        m_undoRedoAction;
                ::MainWorkflow::TrackType   m_trackType;
        };

        NEW_COMMAND( RemoveClips )
        {
            public:
                RemoveClips( WorkflowRenderer* renderer, const QVector<ClipActionInfo>& clipsInfos );
                ~RemoveClips();
                virtual void redo();
                virtual void undo();

            private:
                WorkflowRenderer*               m_renderer;
                QVector<ClipActionInfo>         m_clips;
        };

        NEW_COMMAND( ResizeClip )
        {
            public:
                ResizeClip( ::MainWorkflow* mainWorkflow, const QUuid& uuid, unsigned int trackId,
                            qint64 newBegin, qint64 newEnd,
                            ::MainWorkflow::TrackType trackType );
                virtual void    redo();
                virtual void    undo();
            private:
                ::MainWorkflow*             m_mainWorkflow;
                qint64                      m_oldBegin;
                qint64                      m_oldEnd;
                qint64                      m_newBegin;
                qint64                      m_newEnd;
                Clip*                       m_clip;
                ::MainWorkflow::TrackType   m_trackType;
        };

        NEW_COMMAND( SplitClip )
        {
            public:
                SplitClip( WorkflowRenderer* renderer, Clip* toSplit, uint32_t trackId,
                           qint64 newClipPos, qint64 newClipBegin, ::MainWorkflow::TrackType trackType );
                virtual void    redo();
                virtual void    undo();
            private:
                WorkflowRenderer*           m_renderer;
                Clip*                       m_toSplit;
                Clip*                       m_newClip;
                uint32_t                    m_trackId;
                qint64                      m_newClipPos;
                qint64                      m_newClipBegin;
                qint64                      m_clipOldEnd;
                ::MainWorkflow::TrackType   m_trackType;
        };
    }
}

#endif // COMMANDS_H
