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

#define VIDEOWIDTH 854
#define VIDEOHEIGHT 480

namespace LibVLCpp
{
    class   Media : public Internal<libvlc_media_t>
    {
    public:
        struct          DataCtx
        {
            ~DataCtx();
            QMutex*         mutex;
            Media*          media;
        };
        typedef void    (*lockCallback)(Media::DataCtx* dataCtx, void **pp_ret);
        typedef void    (*unlockCallback)(Media::DataCtx* dataCtx);

        Media(Instance* instance, const QString& filename);
        ~Media();
        void                addOption(const char* opt);
        void                setLockCallback(Media::lockCallback);
        void                setUnlockCallback(Media::unlockCallback);
        void                setDataCtx();
        void                outputInVmem();
        void                outputInWindow();
        void                setPixelBuffer(uchar* buffer);
        uchar*              getPixelBuffer();

    private:
        DataCtx*            buildDataCtx();

        Exception           _ex;
        Instance&           _instance;
        DataCtx*            _dataCtx;
        uchar*              _pixelBuffer;
    };
}

#endif // VLCMEDIA_H
