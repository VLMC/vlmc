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
    : m_pixelBuffer( NULL ), m_fileName( filename )
{
    m_internalPtr = libvlc_media_new( *(LibVLCpp::Instance::getInstance()), filename.toLocal8Bit(), m_ex );
    CheckVlcppException(m_ex);
}

Media::~Media()
{
    libvlc_media_release( m_internalPtr );
}

void                    Media::addOption( const char* opt )
{
    libvlc_media_add_option_flag( m_internalPtr, opt, libvlc_media_option_trusted );
    CheckVlcppException(m_ex);
}

void                    Media::setVideoLockCallback( void* callback )
{
    char    param[64];
    sprintf( param, ":sout-smem-video-prerender-callback=%lld", (qint64)(intptr_t)callback );
    addOption(param);
}

void                    Media::setVideoUnlockCallback( void* callback )
{
    char    param[64];
    sprintf( param, ":sout-smem-video-postrender-callback=%lld", (qint64)(intptr_t)callback );
    addOption( param );
}

void                    Media::setAudioLockCallback( void* callback )
{
    char    param[64];
    sprintf( param, ":sout-smem-audio-prerender-callback=%lld", (qint64)(intptr_t)callback );
    addOption(param);
}

void                    Media::setAudioUnlockCallback( void* callback )
{
    char    param[64];
    sprintf( param, ":sout-smem-audio-postrender-callback=%lld", (qint64)(intptr_t)callback );
    addOption( param );
}

void                    Media::setVideoDataCtx( void* dataCtx )
{
    char    param[64];

    sprintf( param, ":sout-smem-video-data=%lld", (qint64)(intptr_t)dataCtx );
    addOption( param );
}

void                    Media::setAudioDataCtx( void* dataCtx )
{
    char    param[64];

    sprintf( param, ":sout-smem-audio-data=%lld", (qint64)(intptr_t)dataCtx );
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

const QString&          Media::getFileName() const
{
    return m_fileName;
}
