/*****************************************************************************
 * Pool.hpp: Generic pool
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

#ifndef POOL_HPP
#define POOL_HPP

#include <QMutex>
#include <QQueue>

#include "Singleton.hpp"

template <typename T>
class       Pool
{
public:
    Pool()
    {
        m_mutex = new QMutex;
    }
    ~Pool()
    {
        Q_ASSERT( m_pool.empty() == true );
        delete m_mutex;
    }
    T           pop()
    {
        QMutexLocker    lock( m_mutex );
        if ( m_pool.size() == 0 )
        {
            qCritical() << "Pool is empty !!";
            return NULL;
        }
        return m_pool.dequeue();
    }
    const T&        head() const
    {
        QMutexLocker    lock( m_mutex );
        return m_pool.head();
    }
    int     count() const
    {
        QMutexLocker    lock( m_mutex );
        return m_pool.count();
    }

    void    release( T toRelease )
    {
        QMutexLocker    lock( m_mutex );
        m_pool.enqueue( toRelease );
    }
    bool    isEmpty() const
    {
        QMutexLocker    lock( m_mutex );
        return m_pool.empty();
    }
    void    push_back( const T& val )
    {
        QMutexLocker    lock( m_mutex );
        m_pool.push_back( val );
    }
    void    push_front( const T& val )
    {
        QMutexLocker    lock( m_mutex );
        m_pool.push_front( val );
    }
    const T&        last() const
    {
        QMutexLocker    lock( m_mutex );
        return m_pool.last();
    }

private:
    QQueue<T>           m_pool;
    QMutex*             m_mutex;
};

#endif // MEMORYPOOL_HPP

