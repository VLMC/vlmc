#include <QtDebug>
#include "OutputMedia.h"

OutputMedia::OutputMedia( LibVLCpp::Instance* instance ) : Media( instance,"fake://" ), m_pixelBuffer( NULL )
{
    m_dataCtx = new OutputMedia::DataCtx;
    m_dataCtx->mutex = new QMutex;
    m_dataCtx->outputMedia = this;

    char    width[64], height[64], lock[64], unlock[64], data[64];
    sprintf( width, ":invmem-width=%i", VIDEOWIDTH );
    sprintf( height, ":invmem-height=%i", VIDEOHEIGHT );
    sprintf( lock, ":invmem-lock=%lld", (long long int)(intptr_t)&OutputMedia::lock );
    sprintf( unlock, ":invmem-unlock=%lld", (long long int)(intptr_t)&OutputMedia::unlock );
    sprintf( data, ":invmem-data=%lld", (long long int)(intptr_t)m_dataCtx );

    addParam( width );
    addParam( height );
    addParam( lock );
    addParam( unlock );
    addParam( data );
    addParam( ":vout=sdl" );
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
    Media::play();
}
