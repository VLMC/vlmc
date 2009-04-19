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
#include "VLCInstance.h"

using namespace LibVLCpp;

MediaPlayer::MediaPlayer()
{
    m_internalPtr = libvlc_media_player_new( LibVLCpp::Instance::getInstance()->getInternalPtr(), m_ex );
    CheckVlcppException( m_ex );

    // Initialize the event manager
    p_em = libvlc_media_player_event_manager( m_internalPtr, m_ex );

    // Register the callback
    libvlc_event_attach( p_em, libvlc_MediaPlayerSnapshotTaken, callbacks, this, m_ex );
    libvlc_event_attach( p_em, libvlc_MediaPlayerTimeChanged, callbacks, this, m_ex );
    libvlc_event_attach( p_em, libvlc_MediaPlayerPlaying, callbacks, this, m_ex );
    libvlc_event_attach( p_em, libvlc_MediaPlayerPaused, callbacks, this, m_ex );
    libvlc_event_attach( p_em, libvlc_MediaPlayerStopped, callbacks, this, m_ex );
    libvlc_event_attach( p_em, libvlc_MediaPlayerEndReached, callbacks, this, m_ex );
    libvlc_event_attach( p_em, libvlc_MediaPlayerPositionChanged, callbacks, this, m_ex );
}

MediaPlayer::MediaPlayer( Media* media )
{
    m_internalPtr = libvlc_media_player_new_from_media( media->getInternalPtr(), m_ex );
    CheckVlcppException( m_ex );

    // Initialize the event manager
    p_em = libvlc_media_player_event_manager( m_internalPtr, m_ex );

    // Register the callback
    libvlc_event_attach( p_em, libvlc_MediaPlayerSnapshotTaken, callbacks, this, m_ex );
    libvlc_event_attach( p_em, libvlc_MediaPlayerTimeChanged, callbacks, this, m_ex );
    libvlc_event_attach( p_em, libvlc_MediaPlayerPlaying, callbacks, this, m_ex );
    libvlc_event_attach( p_em, libvlc_MediaPlayerPaused, callbacks, this, m_ex );
    libvlc_event_attach( p_em, libvlc_MediaPlayerStopped, callbacks, this, m_ex );
    libvlc_event_attach( p_em, libvlc_MediaPlayerEndReached, callbacks, this, m_ex );
    libvlc_event_attach( p_em, libvlc_MediaPlayerPositionChanged, callbacks, this, m_ex );
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
    case libvlc_MediaPlayerPlaying:
        qDebug() << "Playing event";
        self->emit playing();
        break;
    case libvlc_MediaPlayerPaused:
//        qDebug() << "Paused event";
        self->emit paused();
        break;
    case libvlc_MediaPlayerStopped:
//        qDebug() << "Stopped event";
        self->emit stopped();
//    case libvlc_MediaPlayerForward:
//    case libvlc_MediaPlayerBackward:
        break;
    case libvlc_MediaPlayerEndReached:
        self->emit endReached();
//    case libvlc_MediaPlayerEncounteredError:
        break;
    case libvlc_MediaPlayerTimeChanged:
        //self->timeChangedFilter();
        self->emit timeChanged();
        break;
    case libvlc_MediaPlayerPositionChanged:
        self->emit positionChanged();
        break;
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
    CheckVlcppException( m_ex );
}

void                            MediaPlayer::pause()
{
    qDebug() << "Pausing media";
    libvlc_media_player_pause( m_internalPtr, m_ex );
    CheckVlcppException( m_ex );
}

void                            MediaPlayer::stop()
{
    libvlc_media_player_stop( m_internalPtr, m_ex );
    CheckVlcppException( m_ex );
}

qint64                          MediaPlayer::getTime()
{
    qint64 t = libvlc_media_player_get_time( m_internalPtr, m_ex );
    CheckVlcppException( m_ex );
    return t;
}

void                            MediaPlayer::setTime( qint64 time )
{
    libvlc_media_player_set_time( m_internalPtr, time, m_ex );
    CheckVlcppException( m_ex );
}

float                           MediaPlayer::getPosition()
{
    float p = libvlc_media_player_get_position( m_internalPtr, m_ex );
    CheckVlcppException( m_ex );
    return p;
}

void                            MediaPlayer::setPosition( float pos )
{
    libvlc_media_player_set_position( m_internalPtr, pos, m_ex );
    CheckVlcppException( m_ex );
}

qint64                          MediaPlayer::getLength()
{
    qint64 length = libvlc_media_player_get_length( m_internalPtr, m_ex );
    CheckVlcppException( m_ex );
    return length;
}

void                            MediaPlayer::takeSnapshot( char* outputFile, unsigned int width, unsigned int heigth )
{
    libvlc_video_take_snapshot( *this, outputFile, width, heigth, m_ex );
    CheckVlcppException( m_ex );
}

bool                                MediaPlayer::isPlaying()
{
    int res = libvlc_media_player_is_playing( m_internalPtr, m_ex );
    CheckVlcppException( m_ex );
    return (res == 1);
}

bool                                MediaPlayer::isSeekable()
{
    int res = libvlc_media_player_is_seekable( m_internalPtr, m_ex );
    CheckVlcppException( m_ex );
    return (res == 1);
}

void                                MediaPlayer::setDrawable( void* hwnd )
{
    libvlc_media_player_set_hwnd( m_internalPtr, hwnd, m_ex );
    CheckVlcppException( m_ex );
}

void                                MediaPlayer::setDrawable( uint32_t drawable )
{
    libvlc_media_player_set_xwindow( m_internalPtr, drawable, m_ex );
    CheckVlcppException( m_ex );
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

void                                MediaPlayer::setMedia( Media* media )
{
    libvlc_media_player_set_media( m_internalPtr, media->getInternalPtr(), m_ex);
    CheckVlcppException( m_ex );
}

int                                 MediaPlayer::getWidth()
{
    int width = libvlc_video_get_width( m_internalPtr, m_ex );
    CheckVlcppException( m_ex );
    return width;
}

int                                 MediaPlayer::getHeight()
{
    int height = libvlc_video_get_height( m_internalPtr, m_ex );
    CheckVlcppException( m_ex );
    return height;
}
