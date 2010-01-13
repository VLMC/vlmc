/*****************************************************************************
 * SynchronisationHelper.hpp:   Will help synchronisation with some kind of modified
 *                              wait condition
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

#ifndef SYNCHRONISATIONHELPER_HPP
#define SYNCHRONISATIONHELPER_HPP

#include "WaitCondition.hpp"

class   SynchronisationHelper
{
    public:
        SynchronisationHelper( quint32 nbThreads ) : m_nbThreads( nbThreads )
        {
            m_waitCond = new WaitCondition;
            m_nbAwaken = nbThreads;
        }
        ~SynchronisationHelper()
        {
            delete m_waitCond;
        }
        bool        canBeDeleted()
        {
            QMutexLocker    lock( m_waitCond->getMutex() );
            return m_nbThreads == 0;
        }
        void        release()
        {
            QMutexLocker    lock( m_waitCond->getMutex() );
            --m_nbThreads;
        }

        void        wait()
        {
            QMutexLocker    lock( m_waitCond->getMutex() );
            --m_nbAwaken;
            m_waitCond->waitLocked();
        }
        void        askWake()
        {
            QMutexLocker    lock( m_waitCond->getMutex() );
            ++m_nbAwaken;
            if ( m_nbAwaken == m_nbThreads )
                m_waitCond->wake();
        }

    private:
        quint32             m_nbThreads;
        quint32             m_nbAwaken;
        WaitCondition       *m_waitCond;
};

#endif // SYNCHRONISATIONHELPER_HPP
