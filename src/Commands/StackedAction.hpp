/*****************************************************************************
 * StackedAction.hpp: Stores an action to be executed later
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

#ifndef STACKEDACTION_H
#define STACKEDACTION_H

#include <QUuid>

#include "MainWorkflow.h"

class   StackedAction
{
    public:
        virtual ~StackedAction(){}
        virtual void    execute() = 0;
        virtual bool    isOpposite( const StackedAction* ) const
        {
            return false;
        }
};

class   WorkflowAction : public StackedAction
{
    public:
        WorkflowAction( MainWorkflow* mainWorkflow ) : m_mainWorkflow( mainWorkflow ) {}
    protected:
        MainWorkflow*   m_mainWorkflow;
};

class   TrackAction : public WorkflowAction
{
    public:
        TrackAction( MainWorkflow* mainWorkflow, uint32_t trackId, MainWorkflow::TrackType trackType ) :
            WorkflowAction( mainWorkflow ), m_trackId( trackId ), m_trackType( trackType )
        {
        }
    protected:
        uint32_t                    m_trackId;
        MainWorkflow::TrackType     m_trackType;
};

class   AddClipAction : public TrackAction
{
    public:
        AddClipAction( MainWorkflow* mainWorkflow, uint32_t trackId, MainWorkflow::TrackType trackType,
                       Clip* clip, qint64 startingPos ) : TrackAction( mainWorkflow, trackId, trackType ),
                                                        m_clip( clip ), m_startingPos( startingPos )
        {
        }
        void        execute()
        {
            m_mainWorkflow->addClip( m_clip, m_trackId, m_startingPos, m_trackType );
        }
    protected:
        Clip*       m_clip;
        qint64      m_startingPos;
};

class   RemoveClipAction : public TrackAction
{
    public:
        RemoveClipAction( MainWorkflow* mainWorkflow, uint32_t trackId, MainWorkflow::TrackType trackType,
                       const QUuid& uuid ) : TrackAction( mainWorkflow, trackId, trackType ),
                                            m_uuid( uuid )
        {
        }
        void        execute()
        {
            m_mainWorkflow->removeClip( m_uuid, m_trackId, m_trackType );
        }
    protected:
        QUuid       m_uuid;
};

class   ResizeClipAction : public StackedAction
{
    public:
        ResizeClipAction( Clip* clip, qint64 newBegin, qint64 newEnd ) : m_clip( clip ),
                        m_newBegin( newBegin ), m_newEnd( newEnd )
        {
        }
        void    execute()
        {
            m_clip->setBoundaries( m_newBegin, m_newEnd );
        }
    protected:
        Clip*   m_clip;
        qint64  m_newBegin;
        qint64  m_newEnd;
};

class   PauseAction : public WorkflowAction
{
    public:
        PauseAction( MainWorkflow* mainWorkflow ) : WorkflowAction( mainWorkflow )
        {
        }
        void    execute()
        {
            m_mainWorkflow->pause();
        }
};

class   UnpauseAction : public WorkflowAction
{
    public:
        UnpauseAction( MainWorkflow* mainWorkflow ) : WorkflowAction( mainWorkflow )
        {
        }
        void    execute()
        {
            m_mainWorkflow->unpause();
        }
};

#endif // STACKEDACTION_H
