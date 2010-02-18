/*****************************************************************************
 * MetaDataWorker.h: MetaDataWorker
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
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
namespace LibVLCpp
{
    class   MediaPlayer;
}

class MetaDataWorker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( MetaDataWorker )

    public:
        MetaDataWorker( LibVLCpp::MediaPlayer* mediaPlayer, Media* media );
        ~MetaDataWorker();
        void                        compute();

    private:
        void                        computeDynamicFileMetaData();
        void                        computeImageMetaData();
        void                        prepareAudioSpectrumComputing();
        void                        addAudioValue( int value );
        void                        finalize();

    private:
        void                        metaDataAvailable();
        static void                 lock( MetaDataWorker* metaDataWorker, quint8** pcm_buffer , unsigned int size );
        static void                 unlock( MetaDataWorker* metaDataWorker, quint8* pcm_buffer,
                                        unsigned int channels, unsigned int rate,
                                        unsigned int nb_samples, unsigned int bits_per_sample,
                                        unsigned int size, int pts );

    private:
        LibVLCpp::MediaPlayer*      m_mediaPlayer;
        Media*                      m_media;

        bool                        m_mediaIsPlaying;
        bool                        m_lengthHasChanged;

        unsigned char*              m_audioBuffer;

    private slots:
        void    renderSnapshot();
        void    setSnapshot( const char* );
        void    entrypointPlaying();
        void    entrypointLengthChanged( qint64 );
        void    generateAudioSpectrum();

    signals:
        void    computed();
};

#endif // METADATAWORKER_H
