/*****************************************************************************
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

Exception::errorCallback        Exception::_errorCallback = NULL;
void*                           Exception::_datas = NULL;

Exception::Exception()
{
    this->_internalPtr = new libvlc_exception_t;
    libvlc_exception_init(this->_internalPtr);
}

Exception::~Exception()
{
    this->clear();
    delete this->_internalPtr;
}

void        Exception::setErrorCallback(Exception::errorCallback handler, void* datas)
{
    Exception::_datas = datas;
    Exception::_errorCallback = handler;
}

const char* Exception::getErrorText() const
{
    return libvlc_exception_get_message(this->_internalPtr);
}

void        Exception::clear()
{
    libvlc_exception_clear(this->_internalPtr);
}

int         Exception::raised() const
{
    return libvlc_exception_raised(this->_internalPtr);
}

void        Exception::checkThrow()
{
    if (this->raised() == Exception::Raised)
    {
        if (Exception::_errorCallback != NULL)
            Exception::_errorCallback(this->getErrorText(), Exception::_datas);
        else
            qWarning() << "An exception was raised, but no error handler is set.\nError message is: " << this->getErrorText();
        this->clear();
    }
}
