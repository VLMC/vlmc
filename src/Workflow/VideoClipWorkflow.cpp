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

#include "VideoClipWorkflow.h"
#include "MainWorkflow.h"

VideoClipWorkflow::VideoClipWorkflow( Clip* clip ) : ClipWorkflow( clip )
{
    m_buffer = new LightVideoFrame( MainWorkflow::getInstance()->getWidth()
                                    * MainWorkflow::getInstance()->getHeight()
                                    * Pixel::NbComposantes );
}

VideoClipWorkflow::~VideoClipWorkflow()
{
    delete m_buffer;
}

void            VideoClipWorkflow::initVlcOutput()
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

    if ( m_fullSpeedRender == true )
    {
        m_vlcMedia->addOption( ":no-sout-smem-time-sync" );
    }
    else
        m_vlcMedia->addOption( ":sout-smem-time-sync" );

    sprintf( buffer, ":sout-transcode-width=%i", MainWorkflow::getInstance()->getWidth() );
    m_vlcMedia->addOption( buffer );

    sprintf( buffer, ":sout-transcode-height=%i", MainWorkflow::getInstance()->getHeight() );
    m_vlcMedia->addOption( buffer );

    //Forced output fps
    sprintf( buffer, ":sout-transcode-fps=%f", (float)Clip::DefaultFPS );
    m_vlcMedia->addOption( buffer );
}

void*       VideoClipWorkflow::getLockCallback()
{
    return reinterpret_cast<void*>(&VideoClipWorkflow::lock);
}

void*       VideoClipWorkflow::getUnlockCallback()
{
    return reinterpret_cast<void*>( &VideoClipWorkflow::unlock );
}

void*    VideoClipWorkflow::getOutput()
{
    QMutexLocker    lock( m_renderLock );

    if ( isEndReached() == true )
        return NULL;
    return m_buffer;
}

void    VideoClipWorkflow::lock( VideoClipWorkflow* cw, void** pp_ret, int size )
{
    Q_UNUSED( size );
    cw->m_renderLock->lock();
    *pp_ret = (*(cw->m_buffer))->frame.octets;
}

void    VideoClipWorkflow::unlock( VideoClipWorkflow* cw, void* buffer, int width, int height, int bpp, int size, qint64 pts )
{
    Q_UNUSED( buffer );
    Q_UNUSED( width );
    Q_UNUSED( height );
    Q_UNUSED( bpp );
    Q_UNUSED( size );
    static qint64 previous_pts = pts;
    static qint64 current_pts = pts;

    cw->m_renderLock->unlock();
    cw->m_stateLock->lockForWrite();

    previous_pts = current_pts;
    current_pts = pts;
    current_pts = qMax( current_pts, previous_pts );

    (*(cw->m_buffer))->ptsDiff = current_pts - previous_pts;

    cw->commonUnlock();
}
