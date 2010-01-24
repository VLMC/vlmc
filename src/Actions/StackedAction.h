/*****************************************************************************
 * StackedAction.hpp: Stores an action to be executed later
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
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

#ifndef STACKEDACTION_H
#define STACKEDACTION_H

#include <QUuid>
class   Clip;

#include "MainWorkflow.h"

namespace Action
{
    class   Generic
    {
        public:
            enum    Type
            {
                Pause,
                Unpause,
                Resize,
                Remove,
                Add,
                Move,
            };
            Generic( Type type );
            virtual ~Generic();
            virtual void    execute() = 0;
            virtual bool    isOpposite( const Generic* ) const;
            Type    getType() const;
        private:
            Type    m_type;
    };

    class   Workflow : public Generic
    {
        public:
            Workflow( MainWorkflow* mainWorkflow, Type type );
        protected:
            MainWorkflow*   m_mainWorkflow;
    };

    class   Track : public Workflow
    {
        public:
            Track( MainWorkflow* mainWorkflow, quint32 trackId, MainWorkflow::TrackType trackType, Type type );
        protected:
            quint32                    m_trackId;
            MainWorkflow::TrackType     m_trackType;
    };

    class   AddClip : public Track
    {
        public:
            AddClip( MainWorkflow* mainWorkflow, quint32 trackId, MainWorkflow::TrackType trackType,
                           Clip* clip, qint64 startingPos );
            void        execute();
        protected:
            Clip*       m_clip;
            qint64      m_startingPos;
    };

    class   RemoveClip : public Track
    {
        public:
            RemoveClip( MainWorkflow* mainWorkflow, quint32 trackId, MainWorkflow::TrackType trackType,
                           const QUuid& uuid );
            void        execute();
        protected:
            QUuid       m_uuid;
    };

    class   MoveClip : public Track
    {
        public:
            MoveClip( MainWorkflow* mainWorkflow, const QUuid& uuid, quint32 oldTrack,
                      quint32 newTrack, qint64 newPos, MainWorkflow::TrackType trackType, bool undoRedoAction );
            void    execute();
        private:
            QUuid       m_uuid;
            quint32    m_newTrack;
            qint64      m_newPos;
            bool        m_undoRedoAction;
    };

    class   ResizeClip : public Generic
    {
        public:
            ResizeClip( Clip* clip, qint64 newBegin, qint64 newEnd, bool resizeForSplit );
            void    execute();
        protected:
            Clip*   m_clip;
            qint64  m_newBegin;
            qint64  m_newEnd;
            bool    m_resizeForSplit;
    };

    class   Pause : public Workflow
    {
        public:
            Pause( MainWorkflow* mainWorkflow );
            void    execute();
            bool    isOpposite( const Generic* act ) const;
    };

    class   Unpause : public Workflow
    {
        public:
            Unpause( MainWorkflow* mainWorkflow );
            void    execute();
            bool    isOpposite( const Generic* act ) const;
    };
}
#endif // STACKEDACTION_H
