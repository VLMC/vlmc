/*****************************************************************************
 * MetaDataManmger.cpp: MetaDataManager
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
  * This file contains the MetaDataManager class implementation.
  * It used by the library in Library.[hc]pp.
  * It parse a Clip (see in Clip.[hc]pp) to get meta-data like length, first image, etc.
  */

#include <QtDebug>
#include "MetaDataManager.h"
#include "Library.h"

MetaDataManager::MetaDataManager() : m_renderWidget( NULL ),
    m_mediaIsPlaying( false), m_lengthHasChanged( false )
{
    m_mediaPlayer = new LibVLCpp::MediaPlayer();
    connect( Library::getInstance(), SIGNAL( newMediaLoaded( Media* ) ),this, SLOT( newMediaLoaded( Media* ) ) );
    m_renderWidget = new QWidget();
    m_mediaPlayer->setDrawable( m_renderWidget->winId() );
}

MetaDataManager::~MetaDataManager()
{
    if (m_mediaPlayer)
        delete m_mediaPlayer;
    if (m_renderWidget)
        delete m_renderWidget;
}

void    MetaDataManager::newMediaLoaded( Media* item )
{
    m_mediaList.append( item );

    if ( !isRunning() )
        start();
}

void    MetaDataManager::run()
{
    m_nextMedia = true;
    while ( !m_mediaList.isEmpty() )
    {
        if ( m_nextMedia )
        {
            m_nextMedia = false;

            m_currentClip = m_mediaList.front();
            m_mediaList.pop_front();

            //Disabling audio for this specific use of the media
            m_currentClip->addParam( ":no-audio" );
            m_currentClip->flushParameters();
            //And re-enable it to prevent the audio to be disabled anywhere else.
            m_currentClip->addParam( ":audio" );

            m_mediaPlayer->setMedia( m_currentClip->getVLCMedia() );
            connect( m_mediaPlayer, SIGNAL( lengthChanged() ), this, SLOT( entrypointLengthChanged() ) );
            connect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( entrypointPlaying() ) );
            m_mediaPlayer->play();
        }
        usleep( 10000 );
    }
    return;
}

void    MetaDataManager::getMetaData()
{
    m_mediaIsPlaying = false;
    m_lengthHasChanged = false;

    m_nextMedia = true;
    m_currentClip->setLength( m_mediaPlayer->getLength() );
    m_currentClip->setWidth( m_mediaPlayer->getWidth() );
    m_currentClip->setHeight( m_mediaPlayer->getHeight() );

    //Setting time for snapshot :
    connect( m_mediaPlayer, SIGNAL( positionChanged() ), this, SLOT( renderSnapshot() ) );
    m_mediaPlayer->setTime( m_mediaPlayer->getLength() / 3 );
}

void    MetaDataManager::renderSnapshot()
{
    disconnect( m_mediaPlayer, SIGNAL( positionChanged() ), this, SLOT( renderSnapshot() ) );
    QTemporaryFile tmp;
    tmp.setAutoRemove( false );
    tmp.open();
    m_tmpSnapshotFilename = tmp.fileName();

    connect( m_mediaPlayer, SIGNAL( snapshotTaken() ), this, SLOT( setSnapshot() ) );

    //The slot should be triggered in this methode
    m_mediaPlayer->takeSnapshot( m_tmpSnapshotFilename.toStdString().c_str()
                                 , 32, 32 );
    //Snapshot slot should has been called (but maybe not in next version...)
}

void    MetaDataManager::setSnapshot()
{
    QPixmap* pixmap = new QPixmap( m_tmpSnapshotFilename );
    if ( pixmap->isNull() )
        delete pixmap;
    else
        m_currentClip->setSnapshot( pixmap );
    //TODO : we shouldn't have to do this... patch vlc to get a memory snapshot.
    QFile   tmp( m_tmpSnapshotFilename );
    tmp.remove();

    disconnect( m_mediaPlayer, SIGNAL( snapshotTaken() ), this, SLOT( setSnapshot() ) );

    //CHECKME:
    //This is synchrone, but it may become asynchrone in the future...

    m_mediaPlayer->stop();
    startAudioDataParsing();
}

void    MetaDataManager::startAudioDataParsing()
{
    qDebug() << "Starting audio parsing";
    char    osb[64], psb[64], csb[64], iph[64], data[64];

//    disconnect( m_mediaPlayer, SIGNAL( stopped() ), this, SLOT( startAudioDataParsing() ) );

    //Deactivating video, so that real time doesn't matter
    sprintf( osb, ":amem-opensb=%lld", (long long int)(intptr_t) &MetaDataManager::openSoundBuffer);
    sprintf( psb, ":amem-playsb=%lld", (long long int)(intptr_t) &MetaDataManager::playSoundBuffer);
    sprintf( csb, ":amem-closesb=%lld", (long long int)(intptr_t) &MetaDataManager::closeSoundBuffer);
    sprintf( iph, ":amem-iph=%lld", (long long int)(intptr_t) &MetaDataManager::instanceParameterHandler);
    sprintf( data, ":amem-data=%lld", (long long int)(intptr_t) this);
    m_currentClip->addParam( ":no-video" );
    m_currentClip->addParam( ":audio" );
    m_currentClip->addParam( ":aout=amem" );
    m_currentClip->addParam( osb );
    m_currentClip->addParam( psb );
    m_currentClip->addParam( csb );
    m_currentClip->addParam( iph );
    m_currentClip->addParam( data );
    m_currentClip->flushParameters();

    m_mediaPlayer->setMedia( m_currentClip->getVLCMedia() );
    connect( m_mediaPlayer, SIGNAL( endReached() ), this, SLOT( stopAudioDataParsing() ) );
    qDebug() << "Starting playback again";
    m_mediaPlayer->play();

    //Restoring the clip at a correct value.
    m_currentClip->addParam( ":video" );
}

void    MetaDataManager::stopAudioDataParsing()
{
    qDebug() << "Stopping AudioDataParsing";
    m_mediaPlayer->stop();
}

void    MetaDataManager::openSoundBuffer( void* datas, unsigned int* freq, unsigned int* nbChannels, unsigned int* fourCCFormat, unsigned int* frameSize )
{
    //qDebug() << "Opening sound buffer with freq =" << *freq << "nbChannels =" << *nbChannels << "frameSize =" << *frameSize;
    MetaDataManager::getInstance()->getCurrentMedia()->initAudioData( datas, freq, nbChannels, fourCCFormat, frameSize );
 }

void    MetaDataManager::playSoundBuffer( void* datas, unsigned char* buffer, size_t buffSize, unsigned int nbSample )
{
    //qDebug() << "Playing sound buffer with nbSample =" << nbSample << "buffSize =" << buffSize;
//    qDebug() << "Buff[0] = " << (unsigned int)buffer[0];
    //if (MetaDataManager::getInstance()->getCurrentMedia()->getAudioData()->frameList.size() < 500 )
        MetaDataManager::getInstance()->getCurrentMedia()->addAudioFrame( datas, buffer, buffSize, nbSample );
    //else
//        MetaDataManager::getInstance()->getMediaPlayer()->stop();
}

void    MetaDataManager::closeSoundBuffer( void* datas )
{
    qDebug() << "Closing sound buffer";
    MetaDataManager::getInstance()->getMediaPlayer()->stop();
}

void    MetaDataManager::instanceParameterHandler( void*, char*, char* )
{
}

void    MetaDataManager::entrypointLengthChanged()
{
    disconnect( m_mediaPlayer, SIGNAL( lengthChanged() ), this, SLOT( entrypointLengthChanged() ) );
    m_lengthHasChanged = true;
    if ( m_mediaIsPlaying == true )
        getMetaData();
}

void    MetaDataManager::entrypointPlaying()
{
    disconnect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( entrypointPlaying() ) );
    m_mediaIsPlaying = true;
    if ( m_lengthHasChanged == true )
        getMetaData();
}

