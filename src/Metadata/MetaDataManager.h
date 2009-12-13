/*****************************************************************************
 * MetaDataWorker.h: MetaDataManager
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

#ifndef METADATAMANAGER_H
#define METADATAMANAGER_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QQueue>
#include <QMultiMap>
#include <QThreadPool>
#include "VLCMediaPlayer.h"
#include "Media.h"
#include "Singleton.hpp"

#define DEFAULT_MAX_MEDIA_PLAYER 1

class MetaDataManager : public QObject, public Singleton<MetaDataManager>
{
    Q_OBJECT
    Q_DISABLE_COPY( MetaDataManager )
    enum MediaPlayerState
    {
        Idle,
        Running
    };

    public:
        ~MetaDataManager();
        void    computeMediaMetadata( Media* media );
        void    setMediaPlayersNumberMaxCount( int number );
        void    addMediaPlayer();
        void    removeMediaPlayer();
        void    populateEmptyMediaPlayerSlots();
        int     mediaPlayersMaxCount() const;
        void    checkMediasToCompute();
    private slots:
        void    mediaPlayerIdle( LibVLCpp::MediaPlayer* mediaPlayer );
    private:
        MetaDataManager();

    private:
        QQueue<Media*>                                          m_mediasToComputeMetaData;
        QQueue<Media*>                                          m_mediasToComputeSnapshot;
        QQueue<Media*>                                          m_mediasToComputeAudioSpectrum;
        QMultiMap<MediaPlayerState, LibVLCpp::MediaPlayer*>     m_mediaPlayers;
        QMutex                                                  m_mediasToComputeMetaDataMutex;
        QMutex                                                  m_mediasToComputeSnapshotMutex;
        QMutex                                                  m_mediasToComputeAudioSpectrumMutex;
        QMutex                                                  m_mediaPlayersMutex;
        int                                                     m_mediaPlayersMaxCount;
        int                                                     m_mediaPlayersToRemove;
        friend class                                            Singleton<MetaDataManager>;
};

#endif //METADATAMANAGER_H
