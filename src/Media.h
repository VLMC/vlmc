/*****************************************************************************
 * Media.h: Class for media handling
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

#ifndef MEDIA_H
#define MEDIA_H

#include <QString>
#include <QImage>

#include "VLCMedia.h"
#include "VLCInstance.h"
#include "VLCMediaPlayer.h"

#include "Image.h"

class       Media
{
public:
    Media(const QString& mrl);
    ~Media();

    static void             lock(LibVLCpp::Media::DataCtx* dataCtx, void **pp_ret);
    static void             unlock(LibVLCpp::Media::DataCtx* dataCtx);

    QImage*                 takeSnapshot(unsigned int width, unsigned int heigth);

    bool                    isPlaying();
    bool                    isSeekable();
    qint64                  getLength();
    QImage&                 getImage();

private:
    LibVLCpp::Media*            _vlcMedia;
    LibVLCpp::MediaPlayer*      _vlcMediaPlayer;
    LibVLCpp::Instance*         _instance;
    QString                     _mrl;
    QImage*                     _snapshot;
    uchar*                      _pixelBuffer;
    QImage*                     _image;


};

#endif // MEDIA_H
