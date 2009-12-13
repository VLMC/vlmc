/*****************************************************************************
 * VLCMedia.h: Binding for libvlc_media
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

#ifndef VLCMEDIA_H
#define VLCMEDIA_H

#include "vlc/vlc.h"

#include <QString>
#include <QMutex>

#include "VLCpp.hpp"
#include "VLCException.h"
#include "VLCInstance.h"

namespace LibVLCpp
{
    class   Media : public Internal< libvlc_media_t >
    {
    public:

        Media( const QString& filename );
        ~Media();
        void                addOption( const char* opt );
        void                setVideoLockCallback( void* );
        void                setVideoUnlockCallback( void* );
        void                setAudioLockCallback( void* );
        void                setAudioUnlockCallback( void* );
        void                setVideoDataCtx( void* dataCtx );
        void                setAudioDataCtx( void* dataCtx );
        void                outputInVmem();
        void                outputInWindow();
        void                setPixelBuffer( uchar* buffer );
        uchar*              getPixelBuffer();
        const QString&      getFileName() const;

    private:

    private:

        Exception           m_ex;
        uchar*              m_pixelBuffer;
        QString             m_fileName;
    };
}

#endif // VLCMEDIA_H
