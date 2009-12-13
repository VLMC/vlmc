/*****************``************************************************************
 * Media.cpp: Represents a basic container for media informations.
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

/** \file
  * This file contains the Media class implementation.
  * It contains a VLCMedia and the meta-datas.
  * It's used by the Library
  */

#include <QtDebug>
#include "Media.h"
#include "MetaDataManager.h"

QPixmap*        Media::defaultSnapshot = NULL;
const QString   Media::VideoExtensions = "*.mov *.avi *.mkv *.mpg *.mpeg *.wmv *.mp4 *.ogg *.ogv";
const QString   Media::ImageExtensions = "*.gif *.png *.jpg *.jpeg";
const QString   Media::AudioExtensions = "*.mp3 *.oga *.flac *.aac *.wav";
const QString   Media::streamPrefix = "stream://";

Media::Media( const QString& filePath, const QString& uuid /*= QString()*/ )
    : m_vlcMedia( NULL ),
    m_snapshot( NULL ),
    m_fileInfo( NULL ),
    m_lengthMS( 0 ),
    m_nbFrames( 0 ),
    m_width( 0 ),
    m_height( 0 ),
    m_metadataState( None )
{
    if ( uuid.length() == 0 )
        m_uuid = QUuid::createUuid();
    else
        m_uuid = QUuid( uuid );

    if ( filePath.startsWith( Media::streamPrefix ) == false )
    {
        m_inputType = Media::File;
        m_fileInfo = new QFileInfo( filePath );
        m_fileName = m_fileInfo->fileName();
        setFileType();
        if ( m_fileType == Media::Video || m_fileType == Media::Audio )
            m_mrl = "file:///" + m_fileInfo->absoluteFilePath();
        else
            m_mrl = "fake:///" + m_fileInfo->absoluteFilePath();
    }
    else
    {
        m_inputType = Media::Stream;
        m_mrl = filePath.right( filePath.length() - streamPrefix.length() );
        //FIXME:
        m_fileType = Media::Video;
        m_fileName = m_mrl;
        qDebug() << "Loading a stream";
    }

    m_audioValueList = new QList<int>();
    m_vlcMedia = new LibVLCpp::Media( m_mrl );
}

Media::~Media()
{
    if ( m_vlcMedia )
        delete m_vlcMedia;
    if ( m_snapshot )
        delete m_snapshot;
    if ( m_fileInfo )
        delete m_fileInfo;
}

void        Media::setFileType()
{
    QString filter = "*." + m_fileInfo->suffix().toLower();
    if ( Media::VideoExtensions.contains( filter ) )
        m_fileType = Media::Video;
    else if ( Media::AudioExtensions.contains( filter ) )
        m_fileType = Media::Audio;
    else if ( Media::ImageExtensions.contains( filter ) )
        m_fileType = Media::Image;
    else
        qDebug() << "What the hell is this extension ? And how did you loaded it ?!";
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
    m_lengthMS = length;
}

qint64              Media::getLengthMS() const
{
    return m_lengthMS;
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

float               Media::getFps() const
{
    return m_fps;
}

void                Media::setFps( float fps )
{
    m_fps = fps;
}

Media::FileType     Media::getFileType() const
{
    return m_fileType;
}

void            Media::emitMetaDataComputed( bool hasMetadata )
{
    if ( hasMetadata == true )
        m_metadataState = ParsedWithoutSnapshot;
    emit metaDataComputed( this );
}

void            Media::emitSnapshotComputed()
{
    if ( m_metadataState == ParsedWithoutSnapshot )
        m_metadataState = ParsedWithSnapshot;
    emit snapshotComputed( this );
}

void            Media::emitAudioSpectrumComuted()
{
    m_metadataState = ParsedWithAudioSpectrum;
    emit audioSpectrumComputed( this );
}

Media::InputType    Media::getInputType() const
{
    return m_inputType;
}

void                Media::setUuid( const QUuid& uuid )
{
    m_uuid = uuid;
}

void                Media::setNbFrames( qint64 nbFrames )
{
    m_nbFrames = nbFrames;
}

qint64              Media::getNbFrames() const
{
    return m_nbFrames;
}

const QString&      Media::getMrl() const
{
    return m_mrl;
}

const QString&      Media::getFileName() const
{
    return m_fileName;
}

const QStringList&  Media::getMetaTags() const
{
    return m_metaTags;
}

void                Media::setMetaTags( const QStringList& tags )
{
    m_metaTags = tags;
}

bool                Media::matchMetaTag( const QString& tag ) const
{
    if ( tag.length() == 0 )
        return true;
    QString metaTag;
    foreach ( metaTag, m_metaTags )
    {
        if ( metaTag.startsWith( tag, Qt::CaseInsensitive ) == true )
            return true;
    }
    return false;
}

void            Media::addClip( Clip* clip )
{
    m_clips.insert( clip->getUuid(), clip );
}

void            Media::removeClip( const QUuid& uuid )
{
    m_clips.remove( uuid );
}

Media::MetadataState   Media::getMetadata() const
{
    return m_metadataState;
}
