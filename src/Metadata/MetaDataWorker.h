/*****************************************************************************
 * MetaDataWorker.h: MetaDataWorker
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
 * Authors: Geoffroy Lacarriere <geoffroylaca@gmail.com>
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

#ifndef METADATAWORKER_H
#define METADATAWORKER_H

#include <QList>
#include <QTemporaryFile>
#include <QLabel>
#include "Media.h"
#include "VLCMediaPlayer.h"

class MetaDataWorker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( MetaDataWorker )

    public:
        enum MetaDataType
        {
            MetaData,
            Snapshot,
            Audio
        };

    public:
        MetaDataWorker( LibVLCpp::MediaPlayer* mediaPlayer, Media* media, MetaDataType type );
        ~MetaDataWorker();
        void                        compute();

    private:
        void                        computeVideoMetaData();
        void                        computeImageMetaData();
        void                        computeAudioMetaData();
        void                        addAudioValue( int value );

    private:
        void                        getMetaData();
        void                        initVlcOutput();
        static void                 lock( MetaDataWorker* metaDataWorker, uint8_t** pcm_buffer , unsigned int size );
        static void                 unlock( MetaDataWorker* metaDataWorker, uint8_t* pcm_buffer,
                                        unsigned int channels, unsigned int rate,
                                        unsigned int nb_samples, unsigned int bits_per_sample,
                                        unsigned int size, int pts );

    private:
        LibVLCpp::MediaPlayer*      m_mediaPlayer;
        MetaDataType                m_type;

        Media*                      m_media;
        QString                     m_tmpSnapshotFilename;

        bool                        m_mediaIsPlaying;
        bool                        m_lengthHasChanged;

        unsigned char*              m_audioBuffer;

    signals:
        void    snapshotRequested();
        void    mediaPlayerIdle( LibVLCpp::MediaPlayer* mediaPlayer );

    private slots:
        void    renderSnapshot();
        void    setSnapshot();
        void    entrypointPlaying();
        void    entrypointLengthChanged();
        void    generateAudioSpectrum();
};

#endif // METADATAWORKER_H
