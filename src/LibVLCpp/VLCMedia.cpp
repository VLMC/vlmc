/*****************************************************************************
 * VLCMedia.cpp: Binding for libvlc_media
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
#include <cassert>
#include "VLCMedia.h"

using namespace LibVLCpp;

Media::Media( const QString& filename )
    : m_pixelBuffer( NULL )
{
    m_internalPtr = libvlc_media_new( *(LibVLCpp::Instance::getInstance()), filename.toLocal8Bit(), m_ex );
    CheckVlcppException(m_ex);
}

Media::Media( const Media& media )
{
    clone( media );
}

Media::Media( const Media* media )
{
    clone( *media );
}

Media::~Media()
{
    libvlc_media_release( m_internalPtr );
}

void                    Media::clone( const Media& toClone )
{
    m_pixelBuffer = NULL;
    m_internalPtr = libvlc_media_duplicate( toClone.m_internalPtr );
}

void                    Media::addOption( const char* opt )
{
    libvlc_media_add_option_non_unique( m_internalPtr, opt, m_ex);
    CheckVlcppException(m_ex);
}

void                    Media::setLockCallback( Media::lockCallback callback )
{
    char    param[64];
    sprintf( param, ":vmem-lock=%lld", (qint64)(intptr_t)callback );
    addOption(param);
}

void                    Media::setUnlockCallback( Media::unlockCallback callback )
{
    char    param[64];
    sprintf( param, ":vmem-unlock=%lld", (qint64)(intptr_t)callback );
    addOption( param );
}

void                    Media::setDataCtx( void* dataCtx )
{
    char    param[64];

    sprintf( param, ":vmem-data=%lld", (qint64)(intptr_t)dataCtx );
    addOption( param );
}

void                    Media::outputInVmem()
{
    addOption( ":vout=vmem" );
}

void                    Media::outputInWindow()
{
//    this->addOption();
}

void                    Media::setPixelBuffer( uchar* buffer )
{
    m_pixelBuffer = buffer;
}

uchar*                  Media::getPixelBuffer()
{
    return m_pixelBuffer;
}
