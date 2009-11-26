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

#include <QMap>

MetaDataManager::MetaDataManager() : m_mediaPlayersMaxCount( DEFAULT_MAX_MEDIA_PLAYER ), m_mediaPlayersToRemove( 0 )
{
    for ( int i = 0; i < m_mediaPlayersMaxCount; ++i )
        addMediaPlayer();
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
    m_mediasToComputeMetaData.enqueue( media );
    checkMediasToCompute();
}

void    MetaDataManager::checkMediasToCompute()
{
    QMap<MediaPlayerState, LibVLCpp::MediaPlayer*>::iterator it;
    if ( m_mediasToComputeMetaData.count() > 0 &&
         ( it = m_mediaPlayers.find( Idle ) ) != m_mediaPlayers.end() )
    {
        Media* media;
        LibVLCpp::MediaPlayer* mediaPlayer = it.value();
        m_mediaPlayers.erase( it );
        m_mediaPlayers.insert( Running, mediaPlayer );
        media = m_mediasToComputeMetaData.dequeue();
        MetaDataWorker* worker = new MetaDataWorker( mediaPlayer, media, MetaDataWorker::MetaData );
        connect( worker, SIGNAL( mediaPlayerIdle( LibVLCpp::MediaPlayer* ) ), this, SLOT( mediaPlayerIdle( LibVLCpp::MediaPlayer* ) ) );
        //connect( media, SIGNAL( metaDataComputed( Media* ) ), this, SLOT( checkMediasToCompute() ) );
        worker->compute();
        m_mediasToComputeSnapshot.enqueue( media );
    }
    else if ( m_mediasToComputeSnapshot.count() > 0 &&
              ( it = m_mediaPlayers.find( Idle ) ) != m_mediaPlayers.end() )
    {
        Media* media;
        LibVLCpp::MediaPlayer* mediaPlayer = it.value();
        m_mediaPlayers.erase( it );
        m_mediaPlayers.insert( Running, mediaPlayer );
        media = m_mediasToComputeSnapshot.dequeue();
        MetaDataWorker* worker = new MetaDataWorker( mediaPlayer, media, MetaDataWorker::Snapshot );
        disconnect( media, SIGNAL( metaDataComputed( Media* ) ), this, SLOT( checkMediasToCompute() ) );
        connect( worker, SIGNAL( mediaPlayerIdle( LibVLCpp::MediaPlayer* ) ), this, SLOT( mediaPlayerIdle( LibVLCpp::MediaPlayer* ) ) );
        //connect( media, SIGNAL( snapshotComputed( Media* ) ), this, SLOT( checkMediasToCompute() ) );
        worker->compute();
    }
    return;
}

void    MetaDataManager::mediaPlayerIdle( LibVLCpp::MediaPlayer* mediaPlayer )
{
    m_mediaPlayers.remove( Running, mediaPlayer );
    m_mediaPlayers.insert( Idle, mediaPlayer );
    checkMediasToCompute();
}
