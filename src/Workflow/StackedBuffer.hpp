/*****************************************************************************
* StackedBuffer.hpp: A buffer that's must can be released and pushed back on a Pool.
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

#ifndef STACKEDBUFFER_HPP
#define STACKEDBUFFER_HPP

#include <QtDebug>
#include "Pool.hpp"

template <typename T>
class   StackedBuffer
{
    public:
        StackedBuffer( T buff, bool mustBeReleased  = true ) :
                m_buff( buff ),
                m_mustRelease( mustBeReleased )
        {
        }
        virtual ~StackedBuffer()
        {
        }

        /// \warning    Calling this method will definitely invalidate the pointer;
        virtual void    release() = 0;

        const   T&   get() const
        {
            return m_buff;
        }
        T           get()
        {
            return m_buff;
        }
        operator    const T&() const
        {
            return get();
        }
        operator    T()
        {
            return get();
        }

    protected:
        T           m_buff;
        bool        m_mustRelease;
};

#endif // STACKEDBUFFER_HPP
