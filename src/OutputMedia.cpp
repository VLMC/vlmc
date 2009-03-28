#include <QtDebug>
#include "OutputMedia.h"

OutputMedia::OutputMedia() : m_pixelBuffer( NULL )
{
    char const *vlc_argv[] =
    {
        "-verbose",
        "3",
        "--codec", "invmem",
        //"--snapshot-format", "jpg",
        //"--plugin-path", VLC_TREE "/modules",
        //"--ignore-config", /* Don't use VLC's config files */
    };
    int     vlc_argc = sizeof( vlc_argv ) / sizeof( *vlc_argv );
    
    m_instance = new LibVLCpp::Instance( vlc_argc, vlc_argv );

    m_vlcMedia = new LibVLCpp::Media( m_instance, "fake://" );

    m_dataCtx = new OutputMedia::DataCtx;
    m_dataCtx->mutex = new QMutex;
    m_dataCtx->outputMedia = this;

    char    width[64], height[64], lock[64], unlock[64], data[64];
    sprintf( width, ":invmem-width=%i", VIDEOWIDTH );
    sprintf( height, ":invmem-height=%i", VIDEOHEIGHT );
    sprintf( lock, ":invmem-lock=%lld", (long long int)(intptr_t)&OutputMedia::lock );
    sprintf( unlock, ":invmem-unlock=%lld", (long long int)(intptr_t)&OutputMedia::unlock );
    sprintf( data, ":invmem-data=%lld", (long long int)(intptr_t)m_dataCtx );

    m_vlcMedia->addOption( width );
    m_vlcMedia->addOption( height );
    m_vlcMedia->addOption( lock );
    m_vlcMedia->addOption( unlock );
    m_vlcMedia->addOption( data );
    m_vlcMedia->addOption( ":vout=sdl" );

    m_vlcMediaPlayer = new LibVLCpp::MediaPlayer( m_vlcMedia );
}

uchar*          OutputMedia::lock( OutputMedia::DataCtx* dataCtx )
{
    qDebug() << "Copying into invmem";
    dataCtx->mutex->lock();
    return dataCtx->outputMedia->m_pixelBuffer;
}

void            OutputMedia::unlock( OutputMedia::DataCtx* dataCtx )
{
    //FIXME: use class Image to avoid alloc/free...
    //delete dataCtx->outputMedia->m_pixelBuffer;
    dataCtx->mutex->unlock();
    qDebug() << "Unlocked invmem";
}

void            OutputMedia::setVmem( uchar* pixelBuffer )
{
    m_dataCtx->mutex->lock();
    m_pixelBuffer = pixelBuffer;
    m_dataCtx->mutex->unlock();
}

void            OutputMedia::play()
{
    m_vlcMediaPlayer->play();
}
