#include <QtDebug>
#include <cassert>
#include "VLCMediaPlayer.h"

using namespace LibVLCpp;

//void        MediaPlayer::eventPlayingCallback(const EventManager::Event*, void* data)
//{
//    //getting the event manager back...
//    MediaPlayer* mp = static_cast<MediaPlayer*>(data);
//    //...and removing this dirty callback.
//    //mp->_evMgr->removeEvent(EventManager::MediaPlayingEvent, &eventPlayingCallback, reinterpret_cast<void*>(data));
//    mp->_isReady = true;
//    //stopping the playback must be the last thing we do since it wont continue after this...
//    //mp->stop();
//}

MediaPlayer::MediaPlayer(Media* media, bool playStop /* = true*/) : /*_evMgr(NULL),`*/ _isReady(false)
{
    this->_internalPtr = libvlc_media_player_new_from_media(media->getInternalPtr(), this->_ex);
    this->_ex.checkThrow();
//    if ( playStop == true )
//    {
//        this->_evMgr = new EventManager(this->getInternalPtr());
//        this->_evMgr->addEvent(EventManager::MediaPlayerPositionChangedEvent, &eventPlayingCallback, static_cast<void*>(this));
//    }
}

void                            MediaPlayer::play()
{
    libvlc_media_player_play(this->_internalPtr, this->_ex);
    this->_ex.checkThrow();
}

void                            MediaPlayer::pause()
{
    libvlc_media_player_pause(this->_internalPtr, this->_ex);
    this->_ex.checkThrow();
}

void                            MediaPlayer::stop()
{
    libvlc_media_player_stop(this->_internalPtr, this->_ex);
    this->_ex.checkThrow();
}

qint64                          MediaPlayer::getTime()
{
    qint64 t = libvlc_media_player_get_time(this->_internalPtr, this->_ex);
    this->_ex.checkThrow();
    return t;
}

void                            MediaPlayer::setTime(qint64 time)
{
    libvlc_media_player_set_time(this->_internalPtr, time, this->_ex);
    this->_ex.checkThrow();
}

qint64                          MediaPlayer::getLength()
{
    qint64 length = libvlc_media_player_get_length(this->_internalPtr, this->_ex);
    this->_ex.checkThrow();
    return length;
}

void                            MediaPlayer::takeSnapshot(uchar* output, unsigned int width, unsigned int heigth)
{
    libvlc_video_take_snapshot(*this, reinterpret_cast<char*>(output), width, heigth, this->_ex);
    this->_ex.checkThrow();
}

int                             MediaPlayer::isPlaying()
{
    int res = libvlc_media_player_is_playing(this->_internalPtr, this->_ex);
    this->_ex.checkThrow();
    return res;
}

int                             MediaPlayer::isSeekable()
{
    int res = libvlc_media_player_is_seekable(this->_internalPtr, this->_ex);
    this->_ex.checkThrow();
    return res;
}
