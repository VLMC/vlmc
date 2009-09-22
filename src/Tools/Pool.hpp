/*****************************************************************************
 * Pool.hpp: Generic object pool
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

#ifndef POOL_HPP
#define POOL_HPP

#include <QMutex>
#include <QQueue>

#include "Singleton.hpp"

template <typename T>
class       Pool : public Singleton< Pool<T> >
{
public:
    T*      get()
    {
        QMutexLocker    lock( m_mutex );
        if ( m_pool.size() == 0 )
            return new T;
        return m_pool.dequeue();
    }
    void    release( T* toRelease )
    {
        QMutexLocker    lock( m_mutex );
        m_pool.enqueue( toRelease );
    }
private:
    Pool()
    {
        m_mutex = new QMutex;
    }
    ~Pool()
    {
        while ( m_pool.size() != 0 )
        {
            T*  ptr = m_pool.dequeue();
            delete ptr;
        }
        delete m_mutex;
    }
    QQueue<T*>  m_pool;
    QMutex*     m_mutex;
    friend class Singleton< Pool<T> >;
};

#endif // POOL_HPP
