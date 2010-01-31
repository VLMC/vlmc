/*****************************************************************************
 * MetaDataWorker.cpp: Implement the thread that will get the media informations
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
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
#include <QPainter>
#include <QLabel>
#include <QImage>
#include "vlmc.h"
#include "MetaDataWorker.h"
#include "Library.h"
#include "SettingsManager.h"
#include "VLCMediaPlayer.h"

#include <QThreadPool>
#include <QRunnable>

MetaDataWorker::MetaDataWorker( LibVLCpp::MediaPlayer* mediaPlayer, Media* media ) :
        m_mediaPlayer( mediaPlayer ),
        m_media( media ),
        m_mediaIsPlaying( false),
        m_lengthHasChanged( false ),
        m_audioBuffer( NULL )
{
}

MetaDataWorker::~MetaDataWorker()
{
    if ( m_audioBuffer )
        delete m_audioBuffer;
}

void
MetaDataWorker::compute()
{
    if ( m_media->getFileType() == Media::Video ||
         m_media->getFileType() == Media::Audio )
        computeDynamicFileMetaData();
    else if ( m_media->getFileType() == Media::Image )
        computeImageMetaData();

    m_media->addConstantParam( ":vout=dummy" );
    m_mediaPlayer->setMedia( m_media->getVLCMedia() );
    connect( m_mediaPlayer, SIGNAL( playing() ),
             this, SLOT( entrypointPlaying() ), Qt::QueuedConnection );
    m_mediaPlayer->play();
    m_media->flushVolatileParameters();
}

void
MetaDataWorker::computeDynamicFileMetaData()
{
    //Disabling audio for this specific use of the media
    m_media->addVolatileParam( ":no-audio", ":audio" );
    connect( m_mediaPlayer, SIGNAL( lengthChanged() ),
             this, SLOT( entrypointLengthChanged() ), Qt::QueuedConnection );
}

void
MetaDataWorker::computeImageMetaData()
{
    m_media->addVolatileParam( ":access=fake", ":access=''" );
    m_media->addVolatileParam( ":fake-duration=10000", ":fake-duration=''" );
    //There can't be a length for an image file, so we don't have to wait for it to be updated.
    m_lengthHasChanged = true;
}

void
MetaDataWorker::prepareAudioSpectrumComputing()
{
    m_media->getVLCMedia()->addOption( ":no-sout-video" );
    m_media->getVLCMedia()->addOption( ":sout=#transcode{}:smem" );
    m_media->getVLCMedia()->setAudioDataCtx( this );
    m_media->getVLCMedia()->setAudioLockCallback( reinterpret_cast<void*>( lock ) );
    m_media->getVLCMedia()->setAudioUnlockCallback( reinterpret_cast<void*>( unlock ) );
    m_media->getVLCMedia()->addOption( ":sout-transcode-acodec=fl32" );
    m_media->getVLCMedia()->addOption( ":no-sout-smem-time-sync" );
    m_media->getVLCMedia()->addOption( ":no-sout-keep" );
    connect( m_mediaPlayer, SIGNAL( endReached() ), this, SLOT( generateAudioSpectrum() ), Qt::QueuedConnection );
}

void
MetaDataWorker::metaDataAvailable()
{
    m_mediaIsPlaying = false;
    m_lengthHasChanged = false;

    //In order to wait for the VOUT to be ready:
    //Until we have a way of knowing when it is, both getWidth and getHeight method
    //will trigger exception... so we shut it up.
    if ( m_media->getFileType() != Media::Audio )
    {
        LibVLCpp::Exception::setErrorCallback( LibVLCpp::Exception::silentExceptionHandler );
        while ( m_mediaPlayer->hasVout() == false )
        {
            SleepMS( 1 ); //Ugly isn't it :)
        }
        LibVLCpp::Exception::setErrorCallback( NULL );

        m_media->setWidth( m_mediaPlayer->getWidth() );
        m_media->setHeight( m_mediaPlayer->getHeight() );
        m_media->setFps( m_mediaPlayer->getFps() );
        if ( m_media->getFps() == .0f )
        {
            qWarning() << "Invalid FPS for media:" << m_media->getFileInfo()->absoluteFilePath();
            m_media->setFps( Clip::DefaultFPS );
        }
    }
    else
    {
        const SettingValue *val = SettingsManager::getInstance()->getValue( "project", "VideoProjectFPS" );
        Q_ASSERT_X( val != NULL, "MetaDataWorker", "Can't operate without a project FPS value ");
        m_media->setFps( val->get().toDouble() );
    }
    m_media->setLength( m_mediaPlayer->getLength() );

    m_media->setNbAudioTrack( m_mediaPlayer->getNbAudioTrack() );
    m_media->setNbVideoTrack( m_mediaPlayer->getNbVideoTrack() );
    m_media->setNbFrames( (m_media->getLengthMS() / 1000) * m_media->getFps() );

    m_media->emitMetaDataComputed();
    //Setting time for snapshot :
    if ( m_media->getFileType() == Media::Video ||
         m_media->getFileType() == Media::Image )
    {
        connect( m_mediaPlayer, SIGNAL( positionChanged( float ) ), this, SLOT( renderSnapshot() ) );
        m_mediaPlayer->setTime( m_mediaPlayer->getLength() / 3 );
    }
    else
        finalize();
}

void
MetaDataWorker::renderSnapshot()
{
    if ( m_media->getFileType() == Media::Video ||
         m_media->getFileType() == Media::Audio )
        disconnect( m_mediaPlayer, SIGNAL( positionChanged( float ) ), this, SLOT( renderSnapshot() ) );
    QTemporaryFile tmp;
    tmp.setAutoRemove( false );
    tmp.open();

    connect( m_mediaPlayer, SIGNAL( snapshotTaken( const char* ) ),
             this, SLOT( setSnapshot( const char* ) ), Qt::QueuedConnection );

    //The slot should be triggered in this methode
    m_mediaPlayer->takeSnapshot( tmp.fileName().toStdString().c_str(), 0, 0 );
    //Snapshot slot should has been called (but maybe not in next version...)
}

void
MetaDataWorker::setSnapshot( const char* filename )
{
    QPixmap* pixmap = new QPixmap( filename );
    if ( pixmap->isNull() )
        delete pixmap;
    else
        m_media->setSnapshot( pixmap );
    //TODO : we shouldn't have to do this... patch vlc to get a memory snapshot.
    QFile   tmp( filename );
    tmp.remove();

    disconnect( m_mediaPlayer, SIGNAL( snapshotTaken(const char*) ),
                this, SLOT( setSnapshot( const char* ) ) );

    //CHECKME:
    //This is synchrone, but it may become asynchrone in the future...
//    connect( m_mediaPlayer, SIGNAL( stopped () ), this, SLOT( mediaPlayerStopped() ), Qt::QueuedConnection );

    m_media->emitSnapshotComputed();
    finalize();
}

void
MetaDataWorker::finalize()
{
    m_media->disconnect( this );
    emit    computed();
    delete this;
}

void
MetaDataWorker::entrypointLengthChanged()
{
    disconnect( m_mediaPlayer, SIGNAL( lengthChanged() ), this, SLOT( entrypointLengthChanged() ) );
    m_lengthHasChanged = true;
    if ( m_mediaIsPlaying == true )
        metaDataAvailable();
}

void
MetaDataWorker::entrypointPlaying()
{
    disconnect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( entrypointPlaying() ) );
    m_mediaIsPlaying = true;
    if ( m_lengthHasChanged == true )
        metaDataAvailable();
}

void
MetaDataWorker::lock( MetaDataWorker* metaDataWorker, uint8_t** pcm_buffer , unsigned int size )
{
    if ( metaDataWorker->m_audioBuffer == NULL )
        metaDataWorker->m_audioBuffer = new unsigned char[size];
    *pcm_buffer = metaDataWorker->m_audioBuffer;
}

void
MetaDataWorker::unlock( MetaDataWorker* metaDataWorker, uint8_t* pcm_buffer,
                                      unsigned int channels, unsigned int rate,
                                      unsigned int nb_samples, unsigned int bits_per_sample,
                                      unsigned int size, int pts )
{
    Q_UNUSED( rate );
    Q_UNUSED( size );
    Q_UNUSED( pts );

    int bytePerChannelPerSample = bits_per_sample / 8;

    int leftAverage = 0;
    int rightAverage = 0;

    int it = 0;
    for ( unsigned int i = 0; i < nb_samples; i++)
    {
        int left = 0;
        int right = 0;
        for ( int u = 0 ; u < bytePerChannelPerSample; u++, it++ )
        {
            int increment = 0;
            if ( channels == 2 )
                increment = bytePerChannelPerSample;
            left <<= 8;
            left += pcm_buffer[ it ];
            right <<= 8;
            right += pcm_buffer[ it + increment ];
        }
        leftAverage += left;
        rightAverage += right;
    }
    leftAverage /= nb_samples;
    metaDataWorker->addAudioValue( leftAverage );
}

void
MetaDataWorker::generateAudioSpectrum()
{
    disconnect( m_mediaPlayer, SIGNAL( endReached() ), this, SLOT( generateAudioSpectrum() ) );
    m_mediaPlayer->stop();
//    AudioSpectrumHelper* audioSpectrum = new AudioSpectrumHelper( m_media->getAudioValues() );
//    audioSpectrum->setAutoDelete( true );
//    QThreadPool::globalInstance()->start( audioSpectrum );
    m_media->emitAudioSpectrumComuted();
    delete this;
}

void
MetaDataWorker::addAudioValue( int value )
{
    m_media->getAudioValues()->append( value );
}
