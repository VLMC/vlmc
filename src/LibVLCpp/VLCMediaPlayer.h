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

#include "VLCpp.hpp"
#include "VLCMedia.h"
#include "VLCException.h"

namespace   LibVLCpp
{
    class   MediaPlayer : public Internal< libvlc_media_player_t >
    {
    public:
        MediaPlayer( Media* media, bool playStop = true );
        void                                play();
        void                                pause();
        void                                stop();
        qint64                              getTime();
        void                                setTime( qint64 time );
        qint64                              getLength();
        void                                takeSnapshot( char* outputFile, unsigned int width, unsigned int heigth );
        bool                                isPlaying();
        bool                                isSeekable();
        void                                setDrawable(int handle);

    private:
        Exception                           m_ex;
    };
}

#endif // VLCMEDIAPLAYER_H
