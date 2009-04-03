/*****************************************************************************
 * VLCMediaPlayer.cpp: Binding for libvlc_media_player
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

#include <QtDebug>
#include <cassert>
#include "VLCMediaPlayer.h"

using namespace LibVLCpp;

MediaPlayer::MediaPlayer( Media* media, bool playStop /* = true*/ )
{
    m_internalPtr = libvlc_media_player_new_from_media( media->getInternalPtr(), m_ex );
    m_ex.checkThrow();

    // Initialize the event manager
    p_em = libvlc_media_player_event_manager( m_internalPtr, m_ex );

    // Register the callback
    libvlc_event_attach( p_em, libvlc_MediaPlayerSnapshotTaken, callbacks, this, m_ex );
    libvlc_event_attach( p_em, libvlc_MediaPlayerTimeChanged, callbacks, this, m_ex );
}

/**
 * Event dispatcher.
 */
void                            MediaPlayer::callbacks( const libvlc_event_t* event, void* ptr )
{
    MediaPlayer* self = reinterpret_cast<MediaPlayer*>( ptr );
    switch ( event->type )
    {
//    case libvlc_MediaMetaChanged:
//    case libvlc_MediaSubItemAdded:
//    case libvlc_MediaDurationChanged:
//    case libvlc_MediaPreparsedChanged:
//    case libvlc_MediaFreed:
//    case libvlc_MediaStateChanged:
//    case libvlc_MediaPlayerNothingSpecial:
//    case libvlc_MediaPlayerOpening:
//    case libvlc_MediaPlayerBuffering:
//    case libvlc_MediaPlayerPlaying:
//    case libvlc_MediaPlayerPaused:
//    case libvlc_MediaPlayerStopped:
//    case libvlc_MediaPlayerForward:
//    case libvlc_MediaPlayerBackward:
//    case libvlc_MediaPlayerEndReached:
//    case libvlc_MediaPlayerEncounteredError:
    case libvlc_MediaPlayerTimeChanged:
        self->timeChangedFilter();
        break;
//    case libvlc_MediaPlayerPositionChanged:
//    case libvlc_MediaPlayerSeekableChanged:
//    case libvlc_MediaPlayerPausableChanged:
//    case libvlc_MediaListItemAdded:
//    case libvlc_MediaListWillAddItem:
//    case libvlc_MediaListItemDeleted:
//    case libvlc_MediaListWillDeleteItem:
//    case libvlc_MediaListViewItemAdded:
//    case libvlc_MediaListViewWillAddItem:
//    case libvlc_MediaListViewItemDeleted:
//    case libvlc_MediaListViewWillDeleteItem:
//    case libvlc_MediaListPlayerPlayed:
//    case libvlc_MediaListPlayerNextItemSet:
//    case libvlc_MediaListPlayerStopped:
//    case libvlc_MediaDiscovererStarted:
//    case libvlc_MediaDiscovererEnded:
//    case libvlc_MediaPlayerTitleChanged:
    case libvlc_MediaPlayerSnapshotTaken:
        self->emit snapshotTaken();
        break;
    default:
        break;
    }
}

void                            MediaPlayer::play()
{
    libvlc_media_player_play( m_internalPtr, m_ex );
    m_ex.checkThrow();
}

void                            MediaPlayer::pause()
{
    libvlc_media_player_pause( m_internalPtr, m_ex );
    m_ex.checkThrow();
}

void                            MediaPlayer::stop()
{
    libvlc_media_player_stop( m_internalPtr, m_ex );
    m_ex.checkThrow();
}

qint64                          MediaPlayer::getTime()
{
    qint64 t = libvlc_media_player_get_time( m_internalPtr, m_ex );
    m_ex.checkThrow();
    return t;
}

void                            MediaPlayer::setTime( qint64 time )
{
    libvlc_media_player_set_time( m_internalPtr, time, m_ex );
    m_ex.checkThrow();
}

qint64                          MediaPlayer::getLength()
{
    qint64 length = libvlc_media_player_get_length( m_internalPtr, m_ex );
    m_ex.checkThrow();
    return length;
}

void                            MediaPlayer::takeSnapshot( char* outputFile, unsigned int width, unsigned int heigth )
{
    libvlc_video_take_snapshot( *this, outputFile, width, heigth, m_ex);
    m_ex.checkThrow();
}

bool                                MediaPlayer::isPlaying()
{
    int res = libvlc_media_player_is_playing( m_internalPtr, m_ex );
    m_ex.checkThrow();
    return (res == 1);
}

bool                                MediaPlayer::isSeekable()
{
    int res = libvlc_media_player_is_seekable( m_internalPtr, m_ex );
    m_ex.checkThrow();
    return (res == 1);
}

void                                MediaPlayer::setDrawable( void* hwnd )
{
    libvlc_media_player_set_hwnd( m_internalPtr, hwnd, m_ex );
}

void                                MediaPlayer::setDrawable( uint32_t drawable )
{
    libvlc_media_player_set_xwindow( m_internalPtr, drawable, m_ex );
}

void                                MediaPlayer::timeChangedFilter()
{
    // Don't flood the gui with too many signals
    qint64 currentTime = getTime() / 100;
    static qint64 lastTime = 0;

    if ( currentTime != lastTime )
        emit timeChanged();
    lastTime = currentTime;
}
