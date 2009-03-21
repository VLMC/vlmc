#ifndef VLCMEDIAPLAYER_H
#define VLCMEDIAPLAYER_H

#include "vlc/vlc.h"

#include <QMutex>

#include "VLCpp.hpp"
#include "VLCMedia.h"
#include "VLCException.h"

namespace   LibVLCpp
{
    class   MediaPlayer : public Internal<libvlc_media_player_t>
    {
    public:
        MediaPlayer(Media* media);
        void                                play();
        void                                pause();
        void                                stop();
        qint64                              getTime();
        void                                setTime(qint64 time);
        qint64                              getLength();
    private:
        Exception                           _ex;
    };
}

#endif // VLCMEDIAPLAYER_H
