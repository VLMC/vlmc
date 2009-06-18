/*****************************************************************************
 * MetaDataManmger.h: MetaDataManager
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

/** \file
  * This file contains the MetaDataManager class declaration/definition.
  * It used by the library in Library.[hc]pp.
  * It parse a Clip (see in Clip.[hc]pp) to get meta-data like length, first image, etc.
  */

#ifndef METADATAMANAGER_H
#define METADATAMANAGER_H

#include <QList>
#include <QTemporaryFile>
#include <QThread>
#include <QWidget>
#include "Media.h"
#include "VLCMediaPlayer.h"
#include "Singleton.hpp"

class MetaDataManager : public QThread, public Singleton<MetaDataManager>
{
    Q_OBJECT
    Q_DISABLE_COPY( MetaDataManager )

    friend class Singleton<MetaDataManager>;

    public:
        Media*                  getCurrentMedia() { return m_currentClip; }
        LibVLCpp::MediaPlayer*  getMediaPlayer() { return m_mediaPlayer; }

    private:
        MetaDataManager();
        ~MetaDataManager();

        virtual void                run();

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

        LibVLCpp::MediaPlayer*      m_mediaPlayer;
        QWidget*                    m_renderWidget;

        // TODO: THREAD SAFING
        QList<Media*>               m_mediaList;

        // Thread component
        bool                        m_nextMedia;
        Media*                      m_currentClip;
        //FIXME: Won't work in asynchrone mode
        QString                     m_tmpSnapshotFilename;

        bool                        m_mediaIsPlaying;
        bool                        m_lengthHasChanged;

    private slots:
        void    renderSnapshot();
        void    newMediaLoaded( Media* );
        void    setSnapshot();
        void    startAudioDataParsing();
        void    stopAudioDataParsing();
        void    entrypointPlaying();
        void    entrypointLengthChanged();
};

#endif // METADATAMANAGER_H
