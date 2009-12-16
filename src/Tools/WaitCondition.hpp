/*****************************************************************************
 * WaitCondition.hpp: Small helper to ease thread synchronisation
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

#ifndef WAITCONDITION_HPP
#define WAITCONDITION_HPP

#include <QMutex>
#include <QWaitCondition>

class   WaitCondition
{
    public:
        WaitCondition()
        {
            m_mutex = new QMutex;
            m_waitCond = new QWaitCondition;
        }
        ~WaitCondition()
        {
            delete m_waitCond;
            delete m_mutex;
        }
        void            waitLocked()
        {
            m_waitCond->wait( m_mutex );
        }
        void            wake()
        {
            m_waitCond->wakeAll();
        }
        QMutex*         getMutex()
        {
            return m_mutex;
        }
    private:
        QMutex*         m_mutex;
        QWaitCondition* m_waitCond;
};

#endif // WAITCONDITION_HPP
