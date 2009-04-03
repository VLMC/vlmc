/*****************************************************************************
 * InputMedia.cpp: Class for media handling
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
#include <QTemporaryFile>

#include "InputMedia.h"

InputMedia::InputMedia( const QString& mrl, LibVLCpp::Instance* instance /*= NULL*/ ) :
        Media( instance, mrl ), m_snapshot( NULL ), m_pixelBuffer( NULL ), m_image ( NULL )
{
//    m_vlcMedia->outputInVmem();
//    m_vlcMedia->setLockCallback( InputMedia::lock );
//    m_vlcMedia->setUnlockCallback( InputMedia::unlock );

    char    width[64], height[64], chroma[64], pitch[64];
    sprintf( width, ":vmem-width=%i", VIDEOWIDTH );
    sprintf( height, ":vmem-height=%i", VIDEOHEIGHT );
    sprintf( chroma, ":vmem-chroma=%s", "RV32" );
    sprintf( pitch, ":vmem-pitch=%i", VIDEOWIDTH * 4 );

//    m_vlcMedia->addOption( width );
//    m_vlcMedia->addOption( height );
//    m_vlcMedia->addOption( chroma );
//    m_vlcMedia->addOption( pitch );

//    m_pixelBuffer = new uchar[ VIDEOHEIGHT * VIDEOWIDTH * 4 ];
//    m_image = new QImage( m_pixelBuffer, VIDEOWIDTH, VIDEOHEIGHT, VIDEOWIDTH * 4, QImage::Format_RGB32 );
//    m_image->fill( 0 );

    //Once we got the pixel buffer up and running, we can put it in the "render context"
//    m_vlcMedia->setPixelBuffer( m_pixelBuffer );
//    m_vlcMedia->setDataCtx();
}

InputMedia::~InputMedia()
{
    if ( m_image != NULL )
        delete m_image;
    if ( m_pixelBuffer != NULL)
        delete m_pixelBuffer;
}

void        InputMedia::lock( LibVLCpp::Media::DataCtx* ctx, void **renderPtr )
{
    ctx->mutex->lock();
    *renderPtr = ctx->media->getPixelBuffer();
}

void        InputMedia::unlock( LibVLCpp::Media::DataCtx* ctx )
{
    //qDebug() << "frame just rendered";
    ctx->mutex->unlock();
}

QPixmap*      InputMedia::takeSnapshot( unsigned int width, unsigned int height )
{
    if ( m_snapshot == NULL )
    {
//        qint64 currentTime = m_vlcMediaPlayer->getTime();
//        qint64 length = getLength();
//            qDebug() << currentTime << length;
//        m_vlcMediaPlayer->setTime(length / 2);


//        qDebug() << "trying to take a snapshot";
        QTemporaryFile tmp;
        tmp.open();
        char* tmpStr = const_cast<char*>(tmp.fileName().toStdString().c_str());
        m_vlcMediaPlayer->takeSnapshot( tmpStr, width, height );
//        qDebug() << "done snapshoting";
        m_snapshot = new QPixmap( tmp.fileName() );
//          qDebug() << "written to a QImage";

//        m_vlcMediaPlayer->setTime(currentTime);
    }
    return m_snapshot;
}

bool        InputMedia::isPlaying()
{
    return m_vlcMediaPlayer->isPlaying();
}

bool        InputMedia::isSeekable()
{
    return m_vlcMediaPlayer->isSeekable();
}

QImage&     InputMedia::getImage()
{
    return *m_image;
}

void        InputMedia::play()
{
    Media::play();
}

void        InputMedia::pause()
{
    Media::pause();
}

void        InputMedia::stop()
{
    Media::stop();
}
