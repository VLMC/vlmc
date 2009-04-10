/*****************************************************************************
 * VLCInstance.h: Binding for libvlc instance
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

#ifndef VLCINSTANCE_H
#define VLCINSTANCE_H

#include "vlc/vlc.h"
#include "VLCpp.hpp"
#include "VLCException.h"

namespace LibVLCpp
{
    class   Instance : public Internal< libvlc_instance_t >
    {
    private:
        Instance();
        Instance( int argc, const char** argv );

    private:
        Exception           m_ex;
        static Instance*    m_singleton;
        static Instance*    m_instance;

    public:

        static Instance*    getNewInstance( int argc, const char** argv )
        {
            m_instance = new Instance( argc, argv );
            return m_instance;
        }

        static Instance*    getInstance()
        {
            if (NULL == m_singleton)
                m_singleton =  new Instance();

            return m_singleton;
        }

        static void kill()
        {
            if ( m_singleton != NULL )
            {
                delete m_singleton;
                m_singleton = NULL;
            }
            if ( m_instance != NULL )
            {
                delete m_instance;
                m_instance = NULL;
            }

        }
    };
}

#endif // VLCINSTANCE_H
