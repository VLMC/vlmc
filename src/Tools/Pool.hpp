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

template <typename T, size_t NB_ELEM = 5>
class       Pool : public Singleton< Pool<T, NB_ELEM> >
{
public:
    T*      get()
    {
        QMutexLocker    lock( m_mutex );
        if ( m_pool.size() == 0 )
        {
            return new T;
        }
        uint8_t*    ptr = m_pool.dequeue();
        T*  ret = new (ptr) T;
        return ret;
    }
    void    release( T* toRelease )
    {
        QMutexLocker    lock( m_mutex );
        toRelease->~T();
        m_pool.enqueue( reinterpret_cast<uint8_t*>( toRelease ) );
    }
private:
    Pool()
    {
        for ( size_t i = 0; i < NB_ELEM; ++i )
            m_pool.enqueue( new uint8_t[ sizeof(T) ] );
        m_mutex = new QMutex;
    }
    ~Pool()
    {
        while ( m_pool.size() != 0 )
        {
            uint8_t*  ptr = m_pool.dequeue();
            delete ptr;
        }
        delete m_mutex;
    }
    QQueue<uint8_t*>    m_pool;
    QMutex*             m_mutex;
    friend class Singleton< Pool<T> >;
};

#endif // POOL_HPP

