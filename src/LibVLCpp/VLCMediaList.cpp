#include <QtDebug>

#include "VLCMediaList.h"
#include "VLCInstance.h"

using namespace LibVLCpp;

MediaList::MediaList() : m_listPlayer( NULL )
{
    //Initializing media list :
    m_internalPtr = libvlc_media_list_new( *(Instance::getInstance()), m_exception );
    m_exception.checkThrow();

    //Initializing list_player :
    m_listPlayer = libvlc_media_list_player_new( *(Instance::getInstance()), m_exception );
    m_exception.checkThrow();

    //Associating player with media list :
    libvlc_media_list_player_set_media_list( m_listPlayer, m_internalPtr, m_exception );
    m_exception.checkThrow();

    //Initializing event manager :
    m_eventManager = libvlc_media_list_event_manager( m_internalPtr, m_exception );
    m_exception.checkThrow();
    initWatchedEvents();
}

MediaList::~MediaList()
{

}

void        MediaList::initWatchedEvents()
{
     libvlc_event_attach( m_eventManager, libvlc_MediaListItemAdded, callbacks, this, m_exception );
}

void        MediaList::callbacks( const libvlc_event_t* event, void* ptr)
{
    MediaList* self = reinterpret_cast< MediaList* >( ptr );
    switch ( event->type )
    {
        case libvlc_MediaListItemAdded:
            qDebug() << "Media list added";
            break ;
        default:
            break ;
    }
}

void        MediaList::addMedia( Media* media )
{
    MediaList::Locker   lockMediaList( m_internalPtr );

    libvlc_media_list_add_media( m_internalPtr, *media, m_exception );
    m_exception.checkThrow();
}

void        MediaList::setMediaPlayer( MediaPlayer* mp )
{
    libvlc_media_list_player_set_media_player( m_listPlayer, *mp, m_exception );
}

MediaList::Locker::Locker( MediaList::internalPtr mediaList ) : m_mediaList( mediaList )
{
    libvlc_media_list_lock( mediaList );
}

MediaList::Locker::~Locker()
{
    libvlc_media_list_unlock( m_mediaList );
}
