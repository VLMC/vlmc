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
        NEW_COMMAND( AddClip )
        {
            public:
                AddClip( ::MainWorkflow* workflow, Clip* clip, unsigned int trackNumber, qint64 pos ) :
                        m_workflow( workflow ), m_clip( clip ), m_trackNumber( trackNumber ), m_pos( pos )
                {
                    setText( "Adding clip to track" + QString::number( trackNumber ) );
                }
                virtual void    redo()
                {
                    m_workflow->addClip( m_clip, m_trackNumber, m_pos );
                }
                virtual void    undo()
                {
                    m_workflow->removeClip( m_clip->getUuid(), m_trackNumber );
                }
            private:
                ::MainWorkflow*   m_workflow;
                Clip*           m_clip;
                unsigned int    m_trackNumber;
                qint64          m_pos;
        };
    }
}

#endif // COMMANDS_HPP
