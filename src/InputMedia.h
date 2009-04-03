/*****************************************************************************
 * InputMedia.h: Class for media handling
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


#ifndef INPUTMEDIA_H
#define INPUTMEDIA_H

#include <QString>
#include <QImage>
#include <QThread>

#include "Media.h"
#include "VLCMediaPlayer.h"

#include "Image.h"

class       InputMedia : public Media
{
public:
    InputMedia( const QString& mrl, LibVLCpp::Instance* instance = NULL );
    ~InputMedia();

    static void             lock( LibVLCpp::Media::DataCtx* dataCtx, void **pp_ret );
    static void             unlock( LibVLCpp::Media::DataCtx* dataCtx );

    QImage*                 takeSnapshot( unsigned int width, unsigned int heigth );

    /**
      * Ask libvlc if the media is currently playing
      */
    bool                    isPlaying();
    /**
      * Ask libvlc if the media can be seeked
      */
    bool                    isSeekable();
    /**
      * Can be used to know if the Media is fully usable (IE. can be seeked, vmem can be used, etc...)
      */
    bool                    isReady();
    /**
      * Return the length (duration) of a Media
      */
    qint64                  getLength();
    /**
      * Return the current time of the media
      */
    qint64                  getTime();
    /**
      * Change the current time of the media
      */
    void                    setTime( qint64 time );

    /**
      * Returns the last rendered frame
      */
    QImage&                 getImage();

    virtual void            play();
    virtual void            pause();
    virtual void            stop();
    void                    setDrawable( WId handle );
    LibVLCpp::MediaPlayer*  mediaPlayer() { return m_vlcMediaPlayer; }

private:
    QImage*                     m_snapshot;
    uchar*                      m_pixelBuffer;
    QImage*                     m_image;
};

#endif // INPUTMEDIA_H
