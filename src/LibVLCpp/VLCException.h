/*****************************************************************************
 * VLCException.h: Binding for libvlc_exception
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

#ifndef VLCEXCEPTION_H
#define VLCEXCEPTION_H

#include "vlc/vlc.h"
#include "VLCpp.hpp"

#define CheckVlcppException(ex)     (ex.checkThrow(__FILE__, __LINE__))

namespace LibVLCpp
{
    class   Exception : public Internal< libvlc_exception_t >
    {
    public:
        Exception();
        ~Exception();

        const char*             getErrorText() const;
        void                    clear();
        int                     raised() const;
        void                    checkThrow( const char* file, int line );

        static const int        Raised = 1;
        static const int        NotRaised = 0;

        //error handling part :
        //TODO: have a private error handling in which we could fall back for some specific cases
        typedef void            (*errorCallback)( const char* msg, void* data );
        static  void            setErrorCallback( errorCallback, void* data );

    private:
        static  errorCallback   m_errorCallback;
        static  void*           m_datas;
    };
}

#endif // VLCEXCEPTION_H
