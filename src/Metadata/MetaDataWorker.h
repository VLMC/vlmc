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
#include <QThread>
#include <QWidget>
#include "Media.h"
#include "VLCMediaPlayer.h"

class MetaDataWorker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( MetaDataWorker )

    public:
        MetaDataWorker( Media* media );
        ~MetaDataWorker();
        void                        compute();
        void                        setMediaValidity( bool validity );

    private:
        void                        computeVideoMetaData();
        void                        computeImageMetaData();

        //AMEM part :
        static  void                openSoundBuffer( void* datas, unsigned int* freq,
                                                        unsigned int* nbChannels, unsigned int* fourCCFormat,
                                                        unsigned int* frameSize );
        static  void                playSoundBuffer( void* datas, unsigned char* buffer,
                                                     size_t buffSize, unsigned int nbSample );
        static  void                closeSoundBuffer( void* datas );
        static  void                instanceParameterHandler( void*, char*, char* );

    private:
        void                        getMetaData();

    private:
        LibVLCpp::MediaPlayer*      m_mediaPlayer;
        bool                        m_validity;

        Media*                      m_currentMedia;
        QString                     m_tmpSnapshotFilename;

        bool                        m_mediaIsPlaying;
        bool                        m_lengthHasChanged;

    signals:
        void    snapshotRequested();

    private slots:
        void    renderSnapshot();
        void    setSnapshot();
        void    startAudioDataParsing();
        void    stopAudioDataParsing();
        void    entrypointPlaying();
        void    entrypointLengthChanged();
};

#endif // METADATAWORKER_H
