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

Media::Media( const QString& mrl ) : m_mrl( mrl ), m_snapshot( NULL )
{
    char const *vlc_argv[] =
    {
        "-verbose", "3",
        "--no-skip-frames",
        //"--snapshot-format", "jpg",
        //"--plugin-path", VLC_TREE "/modules",
        //"--ignore-config", /* Don't use VLC's config files */
    };
    int vlc_argc = sizeof( vlc_argv ) / sizeof( *vlc_argv );
    m_instance = new LibVLCpp::Instance( vlc_argc, vlc_argv );

    m_vlcMedia = new LibVLCpp::Media( m_instance, m_mrl );

    m_vlcMedia->outputInVmem();
    m_vlcMedia->setLockCallback( Media::lock );
    m_vlcMedia->setUnlockCallback( Media::unlock );

    char    width[64], height[64], chroma[64], pitch[64];
    sprintf( width, ":vmem-width=%i", VIDEOWIDTH );
    sprintf( height, ":vmem-height=%i", VIDEOHEIGHT );
    sprintf( chroma, ":vmem-chroma=%s", "RV32" );
    sprintf( pitch, ":vmem-pitch=%i", VIDEOWIDTH * 4 );

    m_vlcMedia->addOption( width );
    m_vlcMedia->addOption( height );
    m_vlcMedia->addOption( chroma );
    m_vlcMedia->addOption( pitch );

    m_pixelBuffer = new uchar[ VIDEOHEIGHT * VIDEOWIDTH * 4 ];
    m_image = new QImage( m_pixelBuffer, VIDEOWIDTH, VIDEOHEIGHT, VIDEOWIDTH * 4, QImage::Format_RGB32 );
    m_image->fill( 0 );

    //Once we got the pixel buffer up and running, we can put it in the "render context"
    m_vlcMedia->setPixelBuffer( m_pixelBuffer );
    m_vlcMedia->setDataCtx();

    //And now we play the media
    m_vlcMediaPlayer = new LibVLCpp::MediaPlayer( m_vlcMedia );

    m_isMediaInitialized = false;
    start();
}

void    Media::run()
{
    m_vlcMediaPlayer->play();
    while ( true )
    {
        if( m_vlcMediaPlayer->isSeekable() && m_vlcMediaPlayer->getLength() > 0 )
        {
            m_isMediaInitialized = true;
            m_vlcMediaPlayer->pause();
            return ;
        }
        msleep(100);
    }
}

Media::~Media()
{
    delete m_image;
    delete m_pixelBuffer;
    delete m_vlcMedia;
    delete m_vlcMediaPlayer;
    delete m_instance;
}

void        Media::lock( LibVLCpp::Media::DataCtx* ctx, void **renderPtr )
{
    ctx->mutex->lock();
    *renderPtr = ctx->media->getPixelBuffer();
}

void        Media::unlock( LibVLCpp::Media::DataCtx* ctx )
{
    //qDebug() << "frame just rendered";
    ctx->mutex->unlock();
}

QImage*      Media::takeSnapshot( unsigned int width, unsigned int height )
{
    //FIXME
    //this isn't working, but it seems like a vlc problem, since lastest release of vlc segfault when a screenshot is taken... -_-
    return NULL;


    if ( m_snapshot == NULL )
    {
        qDebug() << "trying to take a snapshot";
        m_vlcMediaPlayer->takeSnapshot( "/tmp/vlmcscreenshot.tmp.gif", width, height );
        qDebug() << "done snapshoting";
        m_snapshot = new QImage( "/tmp/vlmcscreenshot.tmp.gif" );
        qDebug() << "written to a QImage";
    }
    return m_snapshot;
}

bool        Media::isPlaying()
{
    return m_vlcMediaPlayer->isPlaying();
}

bool        Media::isSeekable()
{
    return m_vlcMediaPlayer->isSeekable();
}

qint64      Media::getLength()
{
    return m_vlcMediaPlayer->getLength();
}

void        Media::play()
{
    if( m_isMediaInitialized )
        playSlot();
    else
    {
        msleep(100);
        play();
    }
}

void        Media::playSlot()
{
    m_vlcMediaPlayer->play();
}

QImage&     Media::getImage()
{
    return *m_image;
}

void        Media::setDrawable(int handle)
{
    m_vlcMediaPlayer->setDrawable(handle);
}
