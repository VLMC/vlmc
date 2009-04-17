#include <QtDebug>

#include "VLCMediaList.h"
#include "VLCInstance.h"

using namespace LibVLCpp;

MediaList::MediaList() : m_listPlayer( NULL )
{
    //Initializing media list :
    m_internalPtr = libvlc_media_list_new( *(Instance::getInstance()), m_exception );
    CheckVlcppException(m_exception);

    //Initializing list_player :
    m_listPlayer = libvlc_media_list_player_new( *(Instance::getInstance()), m_exception );
    CheckVlcppException(m_exception);

    //Associating player with media list :
    libvlc_media_list_player_set_media_list( m_listPlayer, m_internalPtr, m_exception );
    CheckVlcppException(m_exception);

    //Initializing event manager :
    m_eventManager = libvlc_media_list_event_manager( m_internalPtr, m_exception );
    CheckVlcppException(m_exception);
    initWatchedEvents();
}

MediaList::~MediaList()
{

}

MediaList::Locker::Locker( MediaList::internalPtr mediaList ) : m_mediaList( mediaList )
{
    libvlc_media_list_lock( mediaList );
}

MediaList::Locker::~Locker()
{
    libvlc_media_list_unlock( m_mediaList );
}

void        MediaList::initWatchedEvents()
{
    libvlc_event_attach( m_eventManager, libvlc_MediaListItemAdded, callbacks, this, m_exception );
    CheckVlcppException( m_exception );
}

void        MediaList::callbacks( const libvlc_event_t* event, void* )
{
    //MediaList* self = reinterpret_cast< MediaList* >( ptr );
    switch ( event->type )
    {
        case libvlc_MediaListPlayerStopped:
            qDebug() << "Playlist stopped";
            break;
        default:
            break ;
    }
}

void        MediaList::addMedia( Media* media )
{
    MediaList::Locker   lockMediaList( m_internalPtr );

    libvlc_media_list_add_media( m_internalPtr, *media, m_exception );
    CheckVlcppException(m_exception);
}

void        MediaList::setMediaPlayer( MediaPlayer* mp )
{
    libvlc_media_list_player_set_media_player( m_listPlayer, *mp, m_exception );
    CheckVlcppException(m_exception);
}

int         MediaList::count()
{
    MediaList::Locker   lockMediaList( m_internalPtr );

    int     nbElems = libvlc_media_list_count( m_internalPtr, m_exception );
    CheckVlcppException( m_exception );
    return nbElems;
}

void        MediaList::clear()
{
    MediaList::Locker   lockMediaList( m_internalPtr );

    //Don't call MediaList::count() since it holds a lock that's already locked just above !
    int     nbElems = libvlc_media_list_count( m_internalPtr, m_exception );
    CheckVlcppException( m_exception );
    for (int i; i < nbElems; ++i)
    {
        libvlc_media_list_remove_index( m_internalPtr, i, m_exception );
        CheckVlcppException( m_exception );
    }
}

void        MediaList::play()
{
    libvlc_media_list_player_play( m_listPlayer, m_exception );
    CheckVlcppException( m_exception );
}

void        MediaList::pause()
{
    libvlc_media_list_player_pause( m_listPlayer, m_exception );
    CheckVlcppException( m_exception );
}

void        MediaList::stop()
{
    libvlc_media_list_player_stop( m_listPlayer, m_exception );
    CheckVlcppException( m_exception );
}
