/*****************************************************************************
 * Media.cpp: Class for media handling
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

#include "Media.h"

Media::Media(const QString& mrl) : _mrl(mrl), _snapshot(NULL)
{
    char const *vlc_argv[] =
    {
        "-verbose",
        "3",
        //"--snapshot-format", "jpg",
        //"--plugin-path", VLC_TREE "/modules",
        //"--ignore-config", /* Don't use VLC's config files */
    };
    int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);
    this->_instance = new LibVLCpp::Instance(vlc_argc, vlc_argv);

    this->_vlcMedia = new LibVLCpp::Media(this->_instance, this->_mrl);

    this->_vlcMedia->outputInVmem();
    this->_vlcMedia->setLockCallback(Media::lock);
    this->_vlcMedia->setUnlockCallback(Media::unlock);

    char    width[64], height[64], chroma[64], pitch[64];
    sprintf(width, ":vmem-width=%i", VIDEOWIDTH);
    sprintf(height, ":vmem-height=%i", VIDEOHEIGHT);
    sprintf(chroma, ":vmem-chroma=%s", "RV32");
    sprintf(pitch, ":vmem-pitch=%i", VIDEOWIDTH * 4);

    this->_vlcMedia->addOption(width);
    this->_vlcMedia->addOption(height);
    this->_vlcMedia->addOption(chroma);
    this->_vlcMedia->addOption(pitch);

    this->_pixelBuffer = new uchar[VIDEOHEIGHT * VIDEOWIDTH * 4];
    this->_image = new QImage(this->_pixelBuffer, VIDEOWIDTH, VIDEOHEIGHT, VIDEOWIDTH * 4, QImage::Format_RGB32);
    this->_image->fill(0);

    //Once we got the pixel buffer up and running, we can put it in the "render context"
    this->_vlcMedia->setPixelBuffer(this->_pixelBuffer);
    this->_vlcMedia->setDataCtx();

    //And now we play the media
    this->_vlcMediaPlayer = new LibVLCpp::MediaPlayer(this->_vlcMedia);
}

Media::~Media()
{
    delete this->_image;
    delete this->_pixelBuffer;
    delete this->_vlcMedia;
    delete this->_vlcMediaPlayer;
    delete this->_instance;
}

void        Media::lock(LibVLCpp::Media::DataCtx* ctx, void **renderPtr)
{
    ctx->mutex->lock();
    *renderPtr = ctx->media->getPixelBuffer();
}

void        Media::unlock(LibVLCpp::Media::DataCtx* ctx)
{
    //    qDebug() << "frame just rendered";
    ctx->mutex->unlock();
}

QImage*      Media::takeSnapshot(unsigned int width, unsigned int height)
{
    //FIXME
    //this isn't working, but it seems like a vlc problem, since lastest release of vlc segfault when a screenshot is taken... -_-
    return NULL;


    if ( this->_snapshot == NULL )
    {
        qDebug() << "trying to take a snapshot";
        this->_vlcMediaPlayer->takeSnapshot("/tmp/vlmcscreenshot.tmp.gif", width, height);
        qDebug() << "done snapshoting";
        this->_snapshot = new QImage("/tmp/vlmcscreenshot.tmp.gif");
        qDebug() << "written to a QImage";
    }
    return this->_snapshot;
}

bool        Media::isPlaying()
{
    return this->_vlcMediaPlayer->isPlaying();
}

bool        Media::isSeekable()
{
    return this->_vlcMediaPlayer->isSeekable();
}

qint64      Media::getLength()
{
    return this->_vlcMediaPlayer->getLength();
}
