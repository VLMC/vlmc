/*****************************************************************************
 * VideoClipWorkflow.cpp : Clip workflow. Will extract a single frame from a VLCMedia
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
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

#include "VideoClipWorkflow.h"
#include "MainWorkflow.h"
#include "StackedBuffer.hpp"
#include "LightVideoFrame.h"
#include "Clip.h"
#include "VLCMedia.h"

#include <QReadWriteLock>

VideoClipWorkflow::VideoClipWorkflow( Clip *clip ) :
        ClipWorkflow( clip ),
        m_lastRenderedFrame( NULL )
{
    for ( unsigned int i = 0; i < VideoClipWorkflow::nbBuffers; ++i )
    {
        m_availableBuffers.enqueue( new LightVideoFrame( MainWorkflow::getInstance()->getWidth(),
                                                         MainWorkflow::getInstance()->getHeight() ) );
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
    if ( m_fullSpeedRender == false )
        m_vlcMedia->addOption( ":sout-smem-time-sync" );
    else
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
VideoClipWorkflow::getLockCallback() const
{
    return reinterpret_cast<void*>(&VideoClipWorkflow::lock);
}

void*
VideoClipWorkflow::getUnlockCallback() const
{
    return reinterpret_cast<void*>( &VideoClipWorkflow::unlock );
}

void*
VideoClipWorkflow::getOutput( ClipWorkflow::GetMode mode )
{
    QMutexLocker    lock( m_renderLock );
    QMutexLocker    lock2( m_computedBuffersMutex );

    if ( preGetOutput() == false )
    {
        if ( m_lastRenderedFrame != NULL )
            return new StackedBuffer( m_lastRenderedFrame, NULL, false );
        return NULL;
    }
    if ( isEndReached() == true )
        return NULL;
    ::StackedBuffer<LightVideoFrame*>* buff;
    if ( mode == ClipWorkflow::Pop )
        buff = new StackedBuffer( m_computedBuffers.dequeue(), this, true );
    else if ( mode == ClipWorkflow::Get )
        buff = new StackedBuffer( m_computedBuffers.head(), NULL, false );
    postGetOutput();
    m_lastRenderedFrame = buff->get();
    return buff;
}

void
VideoClipWorkflow::lock( VideoClipWorkflow *cw, void **pp_ret, int size )
{
    Q_UNUSED( size );
    QMutexLocker        lock( cw->m_availableBuffersMutex );
    LightVideoFrame*    lvf = NULL;

    cw->m_renderLock->lock();
    cw->m_computedBuffersMutex->lock();
    if ( cw->m_availableBuffers.isEmpty() == true )
    {
        lvf = new LightVideoFrame( MainWorkflow::getInstance()->getWidth(),
                                   MainWorkflow::getInstance()->getHeight() );
    }
    else
        lvf = cw->m_availableBuffers.dequeue();
    cw->m_computedBuffers.enqueue( lvf );
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
        m_availableBuffers.enqueue( m_computedBuffers.dequeue() );
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
    if ( m_mustRelease == true && m_poolHandler.isNull() == false )
        m_poolHandler->releaseBuffer( m_buff );
    delete this;
}
