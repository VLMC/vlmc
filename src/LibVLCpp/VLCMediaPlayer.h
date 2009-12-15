/*****************************************************************************
 * VLCMediaPlayer.h: Binding for libvlc_media_player
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

#ifndef VLCMEDIAPLAYER_H
#define VLCMEDIAPLAYER_H

#include "vlc/vlc.h"

#include <QMutex>
#include <QObject>

#include "VLCpp.hpp"
#include "VLCMedia.h"
#include "VLCException.h"

namespace   LibVLCpp
{
    class   MediaPlayer : public QObject, public Internal< libvlc_media_player_t >
    {
        Q_OBJECT
    public:
        MediaPlayer();
        MediaPlayer( Media* media );
        ~MediaPlayer();
        void                                play();
        void                                pause();
        void                                stop();
        qint64                              getTime();
        void                                setTime( qint64 time );
        float                               getPosition();
        void                                setPosition( float pos );
        qint64                              getLength();
        void                                takeSnapshot( const char* outputFile, unsigned int width, unsigned int heigth );
        bool                                isPlaying();
        bool                                isSeekable();
        void                                setDrawable( void* hwnd );
        void                                setDrawable( uint32_t drawable );
        void                                setMedia(Media* media);
        int                                 getWidth();
        int                                 getHeight();
        float                               getFps();
        void                                nextFrame();
        bool                                hasVout();
        const QString&                      getLoadedFileName() const;
        QString                             getLoadedMRL();

    private:
        static void                         callbacks( const libvlc_event_t* event, void* self );

        Exception                           m_ex;
        libvlc_event_manager_t*             p_em;
        Media*                              m_media;

    signals:
        void                                snapshotTaken();
        void                                timeChanged( qint64 );
        void                                playing();
        void                                paused();
        void                                stopped();
        void                                endReached();
        void                                positionChanged( float );
        void                                lengthChanged();
    };
}

#endif // VLCMEDIAPLAYER_H
