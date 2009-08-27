uuuu/*****************************************************************************
 * VLCException.cpp: Binding for libvlc_exception
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

#include <QtDebug>
#include "VLCException.h"

using namespace LibVLCpp;

Exception::errorCallback        Exception::m_errorCallback = NULL;
void*                           Exception::m_datas = NULL;

Exception::Exception()
{
    m_internalPtr = new libvlc_exception_t;
    libvlc_exception_init( m_internalPtr );
}

Exception::~Exception()
{
    clear();
    delete m_internalPtr;
}

void        Exception::setErrorCallback( Exception::errorCallback handler, void* datas /*= NULL*/ )
{
    Exception::m_datas = datas;
    Exception::m_errorCallback = handler;
}

const char* Exception::getErrorText() const
{
//    return libvlc_errmsg();
    return libvlc_exception_get_message( m_internalPtr );
}

void        Exception::clear()
{
    libvlc_exception_clear( m_internalPtr );
}

int         Exception::raised() const
{
    return libvlc_exception_raised( m_internalPtr );
}

void        Exception::checkThrow( const char* file, int line, const char* funcName )
{
    if ( raised() == Exception::Raised )
    {
        if ( Exception::m_errorCallback != NULL )
            Exception::m_errorCallback( getErrorText(), Exception::m_datas );
        else
            qWarning() << "A VLC exception was raised in" << file << ":" << line << '(' << funcName << "):" << getErrorText() << "\"";
        clear();
    }
}
