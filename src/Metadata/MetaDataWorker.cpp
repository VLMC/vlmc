/*****************************************************************************
 * MetaDataWorker.cpp: Implement the thread that will get the media informations
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

#include "vlmc.h"
#include "MetaDataWorker.h"
#include "Library.h"

const int   Clip::DefaultFPS = 30;

MetaDataWorker::MetaDataWorker( LibVLCpp::MediaPlayer* mediaPlayer, Media* media, MetaDataWorker::MetaDataType type ) :
        m_mediaPlayer( mediaPlayer ),
        m_type( type ),
        m_media( media ),
        m_mediaIsPlaying( false),
        m_lengthHasChanged( false )
{
}

MetaDataWorker::~MetaDataWorker()
{
    //if ( m_mediaPlayer->isPlaying() )
    //    m_mediaPlayer->stop();
}

void    MetaDataWorker::compute()
{
    if ( m_media->getFileType() == Media::Video )
    {
        computeVideoMetaData();
    }
    else if ( m_media->getFileType() == Media::Image )
    {
        computeImageMetaData();
    }
    m_media->addConstantParam( ":vout=dummy" );
    m_mediaPlayer->setMedia( m_media->getVLCMedia() );
    connect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( entrypointPlaying() ) );
    m_mediaPlayer->play();
    m_media->flushVolatileParameters();
}

void    MetaDataWorker::computeVideoMetaData()
{
    //Disabling audio for this specific use of the media
    m_media->addVolatileParam( ":no-audio", ":audio" );
    connect( m_mediaPlayer, SIGNAL( lengthChanged() ), this, SLOT( entrypointLengthChanged() ) );
}

void    MetaDataWorker::computeImageMetaData()
{
    m_media->addVolatileParam( ":access=fake", ":access=''" );
    m_media->addVolatileParam( ":fake-duration=10000", ":fake-duration=''" );
    //There can't be a length for an image file, so we don't have to wait for it to be updated.
    m_lengthHasChanged = true;
}

void    MetaDataWorker::getMetaData()
{
    m_mediaIsPlaying = false;
    m_lengthHasChanged = false;

    //In order to wait for the VOUT to be ready:
    //Until we have a way of knowing when it is, both getWidth and getHeight method
    //will trigger exception... so we shut it up.
    LibVLCpp::Exception::setErrorCallback( LibVLCpp::Exception::silentExceptionHandler );
    while ( m_mediaPlayer->hasVout() == false )
    {
        SleepMS( 1 ); //Ugly isn't it :)
    }
    LibVLCpp::Exception::setErrorCallback( NULL );

    if ( m_type == MetaData )
    {
        m_media->setLength( m_mediaPlayer->getLength() );
        m_media->setWidth( m_mediaPlayer->getWidth() );
        m_media->setHeight( m_mediaPlayer->getHeight() );
        m_media->setFps( m_mediaPlayer->getFps() );
        if ( m_media->getFps() == .0f )
        {
            qWarning() << "Invalid FPS for media:" << m_media->getFileInfo()->absoluteFilePath();
            m_media->setFps( Clip::DefaultFPS );
        }
        m_media->setNbFrames( (m_media->getLengthMS() / 1000) * m_media->getFps() );
//        connect( m_mediaPlayer, SIGNAL( stopped () ), this, SLOT( mediaPlayerStopped() ), Qt::QueuedConnection );
        m_mediaPlayer->stop();
        emit mediaPlayerIdle( m_mediaPlayer );
        if ( m_type == Snapshot )
            m_media->emitSnapshotComputed();
        else
            m_media->emitMetaDataComputed( true );
        delete this;
        return;
    }
    else if ( m_type == Snapshot )
    {
        //Setting time for snapshot :
        if ( m_media->getFileType() == Media::Video )
        {
            connect( m_mediaPlayer, SIGNAL( positionChanged() ), this, SLOT( renderSnapshot() ) );
            m_mediaPlayer->setTime( m_mediaPlayer->getLength() / 3 );
        }
        else
            connect( this, SIGNAL( snapshotRequested() ), this, SLOT( renderSnapshot() ) );
    }
}

void    MetaDataWorker::renderSnapshot()
{
    if ( m_media->getFileType() == Media::Video )
        disconnect( m_mediaPlayer, SIGNAL( positionChanged() ), this, SLOT( renderSnapshot() ) );
    else
        disconnect( this, SIGNAL( snapshotRequested() ), this, SLOT( renderSnapshot() ) );
    QTemporaryFile tmp;
    tmp.setAutoRemove( false );
    tmp.open();
    m_tmpSnapshotFilename = tmp.fileName();

    connect( m_mediaPlayer, SIGNAL( snapshotTaken() ), this, SLOT( setSnapshot() ), Qt::QueuedConnection );

    //The slot should be triggered in this methode
    m_mediaPlayer->takeSnapshot( m_tmpSnapshotFilename.toStdString().c_str()
                                 , 0, 0 );
    //Snapshot slot should has been called (but maybe not in next version...)
}

void    MetaDataWorker::setSnapshot()
{
    QPixmap* pixmap = new QPixmap( m_tmpSnapshotFilename );
    if ( pixmap->isNull() )
        delete pixmap;
    else
        m_media->setSnapshot( pixmap );
    //TODO : we shouldn't have to do this... patch vlc to get a memory snapshot.
    QFile   tmp( m_tmpSnapshotFilename );
    tmp.remove();

    disconnect( m_mediaPlayer, SIGNAL( snapshotTaken() ), this, SLOT( setSnapshot() ) );

    //CHECKME:
    //This is synchrone, but it may become asynchrone in the future...
//    connect( m_mediaPlayer, SIGNAL( stopped () ), this, SLOT( mediaPlayerStopped() ), Qt::QueuedConnection );
    m_mediaPlayer->stop();
    emit mediaPlayerIdle( m_mediaPlayer );
    if ( m_type == Snapshot )
        m_media->emitSnapshotComputed();
    else
        m_media->emitMetaDataComputed( true );
    delete this;
    //startAudioDataParsing();
}

void    MetaDataWorker::mediaPlayerStopped()
{
    disconnect( m_mediaPlayer, SIGNAL( stopped() ), this, SLOT( mediaPlayerStopped() ) );
    emit mediaPlayerIdle( m_mediaPlayer );
    if ( m_type == Snapshot )
        m_media->emitSnapshotComputed();
    else
        m_media->emitMetaDataComputed( true );
    delete this;
}

void    MetaDataWorker::startAudioDataParsing()
{
    qDebug() << "Starting audio parsing";
    char    osb[64], psb[64], csb[64], iph[64], data[64];

//    disconnect( m_mediaPlayer, SIGNAL( stopped() ), this, SLOT( startAudioDataParsing() ) );

    sprintf( osb, ":amem-opensb=%lld", (long long int)(intptr_t) &MetaDataWorker::openSoundBuffer);
    sprintf( psb, ":amem-playsb=%lld", (long long int)(intptr_t) &MetaDataWorker::playSoundBuffer);
    sprintf( csb, ":amem-closesb=%lld", (long long int)(intptr_t) &MetaDataWorker::closeSoundBuffer);
    sprintf( iph, ":amem-iph=%lld", (long long int)(intptr_t) &MetaDataWorker::instanceParameterHandler);
    sprintf( data, ":amem-data=%lld", (long long int)(intptr_t) this);
    m_media->addVolatileParam( ":no-video", ":video" );
    m_media->addConstantParam( ":audio" );
    m_media->addVolatileParam( ":aout=amem", ":aout=''" ); //I'm really not sure about this one...
    m_media->addConstantParam( osb );
    m_media->addConstantParam( psb );
    m_media->addConstantParam( csb );
    m_media->addConstantParam( iph );
    m_media->addConstantParam( data );

    m_mediaPlayer->setMedia( m_media->getVLCMedia() );
    m_media->flushVolatileParameters();
    connect( m_mediaPlayer, SIGNAL( endReached() ), this, SLOT( stopAudioDataParsing() ) );
    qDebug() << "Starting playback again";
    m_mediaPlayer->play();
}

void    MetaDataWorker::stopAudioDataParsing()
{
    qDebug() << "Stopping AudioDataParsing";
    m_mediaPlayer->stop();
}

void    MetaDataWorker::openSoundBuffer( void* datas, unsigned int* freq, unsigned int* nbChannels, unsigned int* fourCCFormat, unsigned int* frameSize )
{
    //qDebug() << "Opening sound buffer with freq =" << *freq << "nbChannels =" << *nbChannels << "frameSize =" << *frameSize;
    MetaDataWorker* self = reinterpret_cast<MetaDataWorker*>( datas );
    self->m_media->initAudioData( datas, freq, nbChannels, fourCCFormat, frameSize );
 }

void    MetaDataWorker::playSoundBuffer( void* datas, unsigned char* buffer, size_t buffSize, unsigned int nbSample )
{
    //qDebug() << "Playing sound buffer with nbSample =" << nbSample << "buffSize =" << buffSize;
//    qDebug() << "Buff[0] = " << (unsigned int)buffer[0];
    //if (MetaDataWorker::getInstance()->getCurrentMedia()->getAudioData()->frameList.size() < 500 )
    MetaDataWorker* self = reinterpret_cast<MetaDataWorker*>( datas );
    self->m_media->addAudioFrame( datas, buffer, buffSize, nbSample );
    //else
//        MetaDataWorker::getInstance()->getMediaPlayer()->stop();
}

void    MetaDataWorker::closeSoundBuffer( void* datas )
{
    qDebug() << "Closing sound buffer";
    MetaDataWorker* self = reinterpret_cast<MetaDataWorker*>( datas );

    self->m_mediaPlayer->stop();
}

void    MetaDataWorker::instanceParameterHandler( void*, char*, char* )
{
}

void    MetaDataWorker::entrypointLengthChanged()
{
    disconnect( m_mediaPlayer, SIGNAL( lengthChanged() ), this, SLOT( entrypointLengthChanged() ) );
    m_lengthHasChanged = true;
    if ( m_mediaIsPlaying == true )
        getMetaData();
}

void    MetaDataWorker::entrypointPlaying()
{
    disconnect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( entrypointPlaying() ) );
    m_mediaIsPlaying = true;
    if ( m_lengthHasChanged == true )
        getMetaData();
}
