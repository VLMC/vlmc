/*****************************************************************************
 * Toggleable.hpp : Represents a generic container for toglleable values
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

#ifndef TOGGLEABLE_HPP
#define TOGGLEABLE_HPP

#include <QtDebug>

/**
 *  This class represents a generic toggleable value container.
 */
template <typename T>
class   Toggleable
{
    public:
        Toggleable() : m_ptr( NULL ), m_activated( true ), m_hardDeactivated( false )
        {
        }
        void    setPtr( T ptr )
        {
            Q_ASSERT( m_ptr == NULL );
            m_ptr = ptr;
        }
        operator T()
        {
            return m_ptr;
        }
        T   operator->()
        {
//            Q_ASSERT_X( m_activated == true, "Toggleable<T>::operator->", "Pointer is deactivated" );
            return m_ptr;
        }
        bool    activated() const
        {
            return ( m_hardDeactivated == false && m_activated == true );
        }
        bool    deactivated() const
        {
            return ( m_hardDeactivated == true || m_activated == false );
        }
        bool    hardDeactivated() const
        {
            return m_hardDeactivated;
        }
        void    activate()
        {
            m_activated = true;
        }
        void    deactivate()
        {
            m_activated = false;
        }
        void    setHardDeactivation( bool val )
        {
            m_hardDeactivated = val;
        }

    private:
        T           m_ptr;
        bool        m_activated;
        bool        m_hardDeactivated;
};

#endif // TOGGLEABLE_HPP
