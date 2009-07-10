/*****************``************************************************************
 * Media.cpp: Represents a basic container for media informations.
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
 *	    Geoffroy Lacarriere  <geoffroylaca@gmail.com>		
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

/** \file
  * This file contains the Media class implementation.
  * It contains a VLCMedia and the meta-datas.
  * It's used by the Library
  */

#include <QtDebug>
#include "Media.h"
#include <QPainter>

QPixmap*        Media::defaultSnapshot = NULL;
const QString   Media::VideoExtensions = "*.mov *.avi *.mkv *.mpg *.mpeg *.wmv *.mp4";
const QString   Media::ImageExtensions = "*.gif *.png *.jpg";
const QString   Media::AudioExtensions = "*.mp3 *.oga *.flac *.aac *.wav";

Media::Media( const QFileInfo* fileInfo)
    : m_vlcMedia( NULL ), m_snapshot( NULL ), m_length( 0 ),
    m_width( 0 ), m_height( 0 )
{
    m_uuid = QUuid::createUuid();
    m_fileInfo = new QFileInfo( *fileInfo );
    setFileType();
    if ( m_fileType == Media::Video || m_fileType == Media::Audio )
        m_mrl = "file:///" + fileInfo->absoluteFilePath();
    else
        m_mrl = "fake:///" + fileInfo->absoluteFilePath();
    m_vlcMedia = new LibVLCpp::Media( m_mrl );

    connect(this, SIGNAL(audioFramePointList(QVector<uint>*)), this, SLOT(addAudioFramePointList(QVector<uint>*)));
}

Media::~Media()
{
    if ( m_vlcMedia )
    {
        delete m_vlcMedia;
    }
}

void        Media::setFileType()
{
    QString filter = "*." + m_fileInfo->suffix();
    if ( Media::VideoExtensions.contains( filter ) )
        m_fileType = Media::Video;
    else if ( Media::AudioExtensions.contains( filter ) )
        m_fileType = Media::Audio;
    else if ( Media::ImageExtensions.contains( filter ) )
        m_fileType = Media::Image;
    else
        qDebug() << "What the hell is this extension ? And how did you loaded it ?!";
}

void        Media::loadMedia( const QString& mrl )
{
    if ( m_vlcMedia )
        delete m_vlcMedia;
    m_mrl = mrl;

    m_vlcMedia = new LibVLCpp::Media( mrl );
}

void        Media::flushVolatileParameters()
{
    QString     defaultValue;
    foreach ( defaultValue, m_volatileParameters )
    {
        m_vlcMedia->addOption( defaultValue.toStdString().c_str() );
    }
    m_volatileParameters.clear();
}

void        Media::addVolatileParam( const QString& param, const QString& defaultValue )
{
    m_vlcMedia->addOption( param.toStdString().c_str() );
    m_volatileParameters.append( defaultValue );
}

void        Media::addConstantParam( const QString& param )
{
    m_vlcMedia->addOption( param.toStdString().c_str() );
}

void        Media::setSnapshot( QPixmap* snapshot )
{
    if ( m_snapshot != NULL )
        delete m_snapshot;
    m_snapshot = snapshot;
    emit snapshotChanged();
}

const QPixmap&    Media::getSnapshot() const
{
    if ( m_snapshot != NULL )
        return *m_snapshot;
    if ( Media::defaultSnapshot == NULL )
        Media::defaultSnapshot = new QPixmap( ":/images/images/vlmc.png" );
    return *Media::defaultSnapshot;
}

const QUuid&        Media::getUuid() const
{
    return m_uuid;
}

const QFileInfo*    Media::getFileInfo() const
{
    return m_fileInfo;
}

void                Media::setLength( qint64 length )
{
    m_length = length;
}

qint64              Media::getLength() const
{
    return m_length;
}

int                 Media::getWidth() const
{
    return m_width;
}

void                Media::setWidth( int width )
{
    m_width = width;
}

int                 Media::getHeight() const
{
    return m_height;
}

void                Media::setHeight( int height )
{
    m_height = height;
}

unsigned int        Media::getFps() const
{
    return m_fps;
}

void                Media::setFps( unsigned int fps )
{
    m_fps = fps;
}

Media::FileType     Media::getFileType() const
{
    return m_fileType;
}

void                Media::initAudioData( void* datas, unsigned int* freq, unsigned int* nbChannels, unsigned int* fourCCFormat, unsigned int* frameSize )
{
    m_audioData.freq = *freq;
    m_audioData.nbChannels = *nbChannels;
    m_audioData.frameSize = *frameSize;
    m_audioData.fourCCFormat = *fourCCFormat;
    m_audioData.datas = datas;
}

void                Media::addAudioFrame( void* datas, unsigned char* buffer, size_t buffSize, unsigned int nbSample )
{
    m_audioData.nbSample = nbSample;
    m_audioData.buffSize = buffSize;
    unsigned int bytePerSample = buffSize / nbSample;
    unsigned int bytePerChannelPerSample = bytePerSample / m_audioData.nbChannels;

    int average = 0;
    for (unsigned int i = 0; i < nbSample; i++)
    {
        unsigned int left = 0;
        unsigned int right = 0;
        for ( unsigned int u = 0; u < bytePerChannelPerSample ; u++ )
        {
            int increment = 0;
            if ( m_audioData.nbChannels == 2 )
                increment = 2;
            left <<= 8;
            left += buffer[ u ];
            right <<= 8;
            right += buffer[ u + increment ];
        }
        average += left;
        //qDebug() << "Left: " << left << " Right: " << right;
    }

    static int i = 0;
    average /= nbSample;

    if ( i < 64 )
    {
        //qDebug() << i;
        m_pointList.append( average );
        i++;
    }
    else
    {
        emit audioFramePointList( &m_pointList );
        i = 0;
    }
}

void                        Media::addAudioFramePointList( QVector<uint>* pointList )
{
    QPainter painter;
    QPixmap  pixmap;
    qDebug("new frame");
}
