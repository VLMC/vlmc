#include <QtDebug>
#include <cassert>
#include "VLCMediaPlayer.h"

using namespace LibVLCpp;

MediaPlayer::MediaPlayer(Media* media, bool playStop /* = true*/)
{
    this->_internalPtr = libvlc_media_player_new_from_media(media->getInternalPtr(), this->_ex);
    this->_ex.checkThrow();
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
//    qDebug() << "Setting media time to " << time;
    libvlc_media_player_set_time(this->_internalPtr, time, this->_ex);
    this->_ex.checkThrow();
}

qint64                          MediaPlayer::getLength()
{
    qint64 length = libvlc_media_player_get_length(this->_internalPtr, this->_ex);
    this->_ex.checkThrow();
//    qDebug() << "Media length: " << length;
    return length;
}

void                            MediaPlayer::takeSnapshot(char* outputFile, unsigned int width, unsigned int heigth)
{
    libvlc_video_take_snapshot(*this, outputFile, width, heigth, this->_ex);
    this->_ex.checkThrow();
}

bool                                MediaPlayer::isPlaying()
{
    int res = libvlc_media_player_is_playing(this->_internalPtr, this->_ex);
    this->_ex.checkThrow();
    return (res == 1);
}

bool                                MediaPlayer::isSeekable()
{
    int res = libvlc_media_player_is_seekable(this->_internalPtr, this->_ex);
    this->_ex.checkThrow();
    return (res == 1);
}
