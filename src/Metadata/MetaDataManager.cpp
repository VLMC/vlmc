/*****************************************************************************
 * MetaDataWorker.cpp: MetaDataManager
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Christophe Courtaut <christophe.courtaut@gmail.com>
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

#include "MetaDataManager.h"
#include "MetaDataWorker.h"

#include "vlmc.h"

#include <QMap>
#include <QDebug>

MetaDataManager::MetaDataManager() : m_mediaPlayersMaxCount( DEFAULT_MAX_MEDIA_PLAYER ), m_mediaPlayersToRemove( 0 )
{
    for ( int i = 0; i < m_mediaPlayersMaxCount; ++i )
        addMediaPlayer();
}

MetaDataManager::~MetaDataManager()
{
    while ( m_mediaPlayers.count(Running) )
        SleepMS(1);
    while ( LibVLCpp::MediaPlayer* mediaPlayer = m_mediaPlayers.take( Idle ) )
        delete mediaPlayer;
}

void    MetaDataManager::addMediaPlayer()
{
    if ( m_mediaPlayers.count() <= m_mediaPlayersMaxCount )
        m_mediaPlayers.insert( Idle, new LibVLCpp::MediaPlayer() );
}

void    MetaDataManager::removeMediaPlayer()
{
    if ( m_mediaPlayers.count() > 0 )
    {
        QMap<MediaPlayerState, LibVLCpp::MediaPlayer*>::iterator it = m_mediaPlayers.find( Idle );
        if ( it != m_mediaPlayers.end() )
        {
            delete it.value();
            m_mediaPlayers.erase( it );
        }
        else
            ++m_mediaPlayersToRemove;
    }
}

void    MetaDataManager::setMediaPlayersNumberMaxCount( int number )
{
    if ( number <= 0 )
        return;
    if ( number <= m_mediaPlayers.count() )
    {
        for ( int i = m_mediaPlayers.count() - number; i > 0; --i )
            removeMediaPlayer();
    }
    m_mediaPlayersMaxCount = number;
}

void    MetaDataManager::populateEmptyMediaPlayerSlots()
{
    if ( m_mediaPlayers.count() < m_mediaPlayersMaxCount )
    {
        for ( int i = m_mediaPlayersMaxCount - m_mediaPlayers.count(); i > 0; --i )
            addMediaPlayer();
    }
}

int     MetaDataManager::mediaPlayersMaxCount() const
{
    return m_mediaPlayers.count();
}

void    MetaDataManager::computeMediaMetadata( Media *media )
{
    {
        QMutexLocker lock(&m_mediasToComputeMetaDataMutex);
        m_mediasToComputeMetaData.enqueue( media );
    }
    checkMediasToCompute();
}

void    MetaDataManager::checkMediasToCompute()
{
    //qDebug() << "checking media to compute" << m_mediasToComputeMetaData << m_mediasToComputeSnapshot << m_mediasToComputeAudioSpectrum;
    m_mediasToComputeMetaDataMutex.lock();
    m_mediasToComputeSnapshotMutex.lock();
    m_mediasToComputeAudioSpectrumMutex.lock();
    m_mediaPlayersMutex.lock();
    QMap<MediaPlayerState, LibVLCpp::MediaPlayer*>::iterator it;
    if ( m_mediasToComputeMetaData.count() > 0 &&
         ( it = m_mediaPlayers.find( Idle ) ) != m_mediaPlayers.end() )
    {
        m_mediasToComputeAudioSpectrumMutex.unlock();
        Media* media;
        media = m_mediasToComputeMetaData.dequeue();
        m_mediasToComputeSnapshot.enqueue( media );
        m_mediasToComputeMetaDataMutex.unlock();
        m_mediasToComputeSnapshotMutex.unlock();
        LibVLCpp::MediaPlayer* mediaPlayer = it.value();
        m_mediaPlayers.erase( it );
        m_mediaPlayers.insert( Running, mediaPlayer );
        m_mediaPlayersMutex.unlock();
        MetaDataWorker* worker = new MetaDataWorker( mediaPlayer, media, MetaDataWorker::MetaData );
        connect( worker, SIGNAL( mediaPlayerIdle( LibVLCpp::MediaPlayer* ) ), this, SLOT( mediaPlayerIdle( LibVLCpp::MediaPlayer* ) ), Qt::DirectConnection );
        //connect( media, SIGNAL( metaDataComputed( Media* ) ), this, SLOT( checkMediasToCompute() ) );
        worker->compute();
    }
    else if ( m_mediasToComputeSnapshot.count() > 0 &&
              ( it = m_mediaPlayers.find( Idle ) ) != m_mediaPlayers.end() )
    {
        m_mediasToComputeMetaDataMutex.unlock();
        Media* media;
        media = m_mediasToComputeSnapshot.dequeue();
        m_mediasToComputeAudioSpectrum.enqueue( media );
        m_mediasToComputeSnapshotMutex.unlock();
        m_mediasToComputeAudioSpectrumMutex.unlock();
        LibVLCpp::MediaPlayer* mediaPlayer = it.value();
        m_mediaPlayers.erase( it );
        m_mediaPlayers.insert( Running, mediaPlayer );
        m_mediaPlayersMutex.unlock();
        MetaDataWorker* worker = new MetaDataWorker( mediaPlayer, media, MetaDataWorker::Snapshot );
        //disconnect( media, SIGNAL( metaDataComputed( Media* ) ), this, SLOT( checkMediasToCompute() ) );
        connect( worker, SIGNAL( mediaPlayerIdle( LibVLCpp::MediaPlayer* ) ), this, SLOT( mediaPlayerIdle( LibVLCpp::MediaPlayer* ) ), Qt::DirectConnection );
        //connect( media, SIGNAL( snapshotComputed( Media* ) ), this, SLOT( checkMediasToCompute() ) );
        worker->compute();
    }
    else if (m_mediasToComputeAudioSpectrum.count() > 0 &&
             (it = m_mediaPlayers.find( Idle ) ) != m_mediaPlayers.end() )
    {
        m_mediasToComputeMetaDataMutex.unlock();
        m_mediasToComputeSnapshotMutex.unlock();
        Media* media;
        media = m_mediasToComputeAudioSpectrum.dequeue();
        m_mediasToComputeAudioSpectrumMutex.unlock();
        LibVLCpp::MediaPlayer* mediaPlayer = it.value();
        m_mediaPlayers.erase( it );
        m_mediaPlayers.insert( Running, mediaPlayer );
        m_mediaPlayersMutex.unlock();
        /******************************************************************************************/
        // FIXME: In MetaDataWorker the m_media->getVLCMedia()->setAudioDataCtx( this ); method
        // doesn't change anything so the lock continue to use the old instance even if this method
        // is set with a new instance. It seems that the mediaPlayer instance keept the first value
        // and don't care of the new value.
        mediaPlayer->stop();
        delete mediaPlayer;
        mediaPlayer = new LibVLCpp::MediaPlayer();
        /******************************************************************************************/
        MetaDataWorker* worker = new MetaDataWorker( mediaPlayer, media, MetaDataWorker::Audio );
        connect( worker, SIGNAL( mediaPlayerIdle( LibVLCpp::MediaPlayer* ) ), this, SLOT( mediaPlayerIdle( LibVLCpp::MediaPlayer* ) ), Qt::DirectConnection );
        worker->compute();
    }
    else
    {
        m_mediasToComputeSnapshotMutex.unlock();
        m_mediasToComputeMetaDataMutex.unlock();
        m_mediasToComputeAudioSpectrumMutex.unlock();
        m_mediaPlayersMutex.unlock();
    }
    return;
}

void    MetaDataManager::mediaPlayerIdle( LibVLCpp::MediaPlayer* mediaPlayer )
{
    //qDebug() << "new media player idle";
    {
        QMutexLocker lock(&m_mediaPlayersMutex);
        m_mediaPlayers.remove( Running, mediaPlayer );
        m_mediaPlayers.insert( Idle, mediaPlayer );
    }
    checkMediasToCompute();
}
