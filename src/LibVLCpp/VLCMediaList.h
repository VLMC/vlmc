#ifndef VLCMEDIALIST_H
#define VLCMEDIALIST_H

#include "vlc/vlc.h"
#include "VLCException.h"
#include "VLCpp.hpp"
#include "VLCMediaPlayer.h"

namespace   LibVLCpp
{
    class       MediaList : public Internal<libvlc_media_list_t>
    {
    public:
        class   Locker
        {
        public:
            Locker( MediaList::internalPtr mediaList );
            ~Locker();
        private:
            MediaList::internalPtr     m_mediaList;
        };
        MediaList();
        ~MediaList();

        void                            addMedia( Media* media );
        void                            setMediaPlayer( MediaPlayer* mp );
        static void                     callbacks( const libvlc_event_t* event, void* self );
    private:
        void                            initWatchedEvents();

        MediaPlayer*                    m_vlcMediaPlayer;
        Exception                       m_exception;
        libvlc_media_list_player_t*     m_listPlayer;
        libvlc_event_manager_t*         m_eventManager;
    };
}

#endif // VLCMEDIALIST_H
