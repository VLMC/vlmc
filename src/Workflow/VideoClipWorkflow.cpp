/*****************************************************************************
 * VideoClipWorkflow.cpp : Clip workflow. Will extract a single frame from a VLCMedia
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

#include "vlmc.h"
#include "VideoClipWorkflow.h"
#include "MainWorkflow.h"
#include "StackedBuffer.hpp"
#include "LightVideoFrame.h"
#include "Clip.h"

VideoClipWorkflow::VideoClipWorkflow( Clip *clip ) :
        ClipWorkflow( clip )
{
    for ( unsigned int i = 0; i < VideoClipWorkflow::nbBuffers; ++i )
    {
        m_availableBuffers.enqueue( new LightVideoFrame( MainWorkflow::getInstance()->getWidth()
                                    * MainWorkflow::getInstance()->getHeight()
                                    * Pixel::NbComposantes ) );
    }
    debugType = 2;
}

VideoClipWorkflow::~VideoClipWorkflow()
{
    while ( m_availableBuffers.isEmpty() == false )
        delete m_availableBuffers.dequeue();
    while ( m_computedBuffers.isEmpty() == false )
        delete m_computedBuffers.dequeue();
}

void
VideoClipWorkflow::initVlcOutput()
{
    char        buffer[32];

    m_vlcMedia->addOption( ":no-audio" );
    m_vlcMedia->addOption( ":no-sout-audio" );
    m_vlcMedia->addOption( ":sout=#transcode{}:smem" );
    m_vlcMedia->setVideoDataCtx( this );
    m_vlcMedia->setVideoLockCallback( reinterpret_cast<void*>( getLockCallback() ) );
    m_vlcMedia->setVideoUnlockCallback( reinterpret_cast<void*>( getUnlockCallback() ) );
    m_vlcMedia->addOption( ":sout-transcode-vcodec=RV24" );
    m_vlcMedia->addOption( ":sout-transcode-acodec=s16l" );
//    m_vlcMedia->addOption( ":no-sout-keep" );

    m_vlcMedia->addOption( ":no-sout-smem-time-sync" );

    sprintf( buffer, ":sout-transcode-width=%i",
             MainWorkflow::getInstance()->getWidth() );
    m_vlcMedia->addOption( buffer );
    sprintf( buffer, ":sout-transcode-height=%i",
             MainWorkflow::getInstance()->getHeight() );
    m_vlcMedia->addOption( buffer );
    sprintf( buffer, ":sout-transcode-fps=%f", (float)Clip::DefaultFPS );
    m_vlcMedia->addOption( buffer );
}

void*
VideoClipWorkflow::getLockCallback()
{
    return reinterpret_cast<void*>(&VideoClipWorkflow::lock);
}

void*
VideoClipWorkflow::getUnlockCallback()
{
    return reinterpret_cast<void*>( &VideoClipWorkflow::unlock );
}

void*
VideoClipWorkflow::getOutput( ClipWorkflow::GetMode mode )
{
//    qDebug() << "entering video get output";
    QMutexLocker    lock( m_renderLock );
//    qDebug() << "got video render lock";
    QMutexLocker    lock2( m_computedBuffersMutex );
//    qDebug() << "got video computer buffers lock";

    if ( preGetOutput() == false )
    {
//        qDebug() << "video preGetOutput() returned false";
        return NULL;
    }
//    qWarning() << "Video::getOutput(). Available:" << m_availableBuffers.count() <<
//    "Computed:" << m_computedBuffers.count();
    if ( isEndReached() == true )
    {
//        qDebug() << "video end reached was true";
        return NULL;
    }
    ::StackedBuffer<LightVideoFrame*>* buff;
    if ( mode == ClipWorkflow::Pop )
    {
        buff = new StackedBuffer( m_computedBuffers.dequeue(), this, true );
    }
    else if ( mode == ClipWorkflow::Get )
        buff = new StackedBuffer( m_computedBuffers.head(), NULL, false );
//    qDebug() << "calling video postGetOutput();";
    postGetOutput();
//    qDebug() << "returning videobuff";
    return buff;
}

void
VideoClipWorkflow::lock( VideoClipWorkflow *cw, void **pp_ret, int size )
{
    Q_UNUSED( size );
    QMutexLocker        lock( cw->m_availableBuffersMutex );
    cw->m_renderLock->lock();
    cw->m_computedBuffersMutex->lock();

    LightVideoFrame*    lvf = NULL;
    if ( cw->m_availableBuffers.isEmpty() == true )
    {
//        qCritical() << "Late buffer generation. Spawning new video buffer";
        lvf = new LightVideoFrame( MainWorkflow::getInstance()->getWidth()
                                   * MainWorkflow::getInstance()->getHeight()
                                   * Pixel::NbComposantes );
    }
    else
        lvf = cw->m_availableBuffers.dequeue();
    cw->m_computedBuffers.enqueue( lvf );
//    qWarning() << ">>>VideoGeneration. Available:" <<
//        cw->m_availableBuffers.count() << "Computed:" << cw->m_computedBuffers.count();
//    qWarning() << "feeding video buffer";
    *pp_ret = (*(lvf))->frame.octets;
}

void
VideoClipWorkflow::unlock( VideoClipWorkflow *cw, void *buffer, int width,
                           int height, int bpp, int size, qint64 pts )
{
    Q_UNUSED( buffer );
    Q_UNUSED( width );
    Q_UNUSED( height );
    Q_UNUSED( bpp );
    Q_UNUSED( size );

    cw->computePtsDiff( pts );
    LightVideoFrame     *lvf = cw->m_computedBuffers.last();
    (*(lvf))->ptsDiff = cw->m_currentPts - cw->m_previousPts;
//    qWarning() << "Computed ptsDiff:" << (*(lvf))->ptsDiff;
//    if ( (*(lvf))->ptsDiff > 100000 )
//    {
//        qWarning() << "Probably invalid pts diff. pts:" << pts << "m_currentPts:" <<
//    cw->m_currentPts << "m_previousPts:" << cw->m_previousPts
//                << "state:" << cw->m_state;
//    }
    // If this is the first buffer that has been rendered,
    // there may be a waiting TrackWorkflow.
    cw->commonUnlock();
    cw->m_renderLock->unlock();
    cw->m_computedBuffersMutex->unlock();
}

uint32_t
VideoClipWorkflow::getNbComputedBuffers() const
{
    return m_computedBuffers.count();
}

uint32_t
VideoClipWorkflow::getMaxComputedBuffers() const
{
    return VideoClipWorkflow::nbBuffers;
}

void
VideoClipWorkflow::releaseBuffer( LightVideoFrame *lvf )
{
    QMutexLocker    lock( m_availableBuffersMutex );
    m_availableBuffers.enqueue( lvf );
}

void
VideoClipWorkflow::flushComputedBuffers()
{
    QMutexLocker    lock( m_computedBuffersMutex );
    QMutexLocker    lock2( m_availableBuffersMutex );

    while ( m_computedBuffers.isEmpty() == false )
    {
        m_availableBuffers.enqueue( m_computedBuffers.dequeue() );
    }
}

VideoClipWorkflow::StackedBuffer::StackedBuffer( LightVideoFrame *lvf,
                                                    VideoClipWorkflow *poolHandler,
                                                    bool mustBeReleased) :
    ::StackedBuffer<LightVideoFrame*>( lvf, mustBeReleased ),
    m_poolHandler( poolHandler )
{
}

void
VideoClipWorkflow::StackedBuffer::release()
{
    if ( m_mustRelease == true )
        m_poolHandler->releaseBuffer( m_buff );
    delete this;
}
