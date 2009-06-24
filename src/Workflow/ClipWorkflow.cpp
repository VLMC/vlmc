/*****************************************************************************
 * ClipWorkflow.cpp : Clip workflow. Will extract a single frame from a VLCMedia
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

#include <QtDebug>

#include "vlmc.h"
#include "ClipWorkflow.h"

int     g_debugId = 0;

ClipWorkflow::ClipWorkflow( Clip::Clip* clip ) :
                m_clip( clip ),
                m_buffer( NULL ),
                m_usingBackBuffer( false ),
                m_mediaPlayer(NULL),
                m_state( ClipWorkflow::Stopped ),
                m_requiredState( ClipWorkflow::None )
{
    m_buffer = new unsigned char[VIDEOHEIGHT * VIDEOWIDTH * 4];
    m_backBuffer = new unsigned char[VIDEOHEIGHT * VIDEOWIDTH * 4];
    m_stateLock = new QReadWriteLock;
    m_requiredStateLock = new QMutex;
    m_condMutex = new QMutex;
    m_waitCond = new QWaitCondition;
    m_backBufferLock = new QReadWriteLock;

    this->debugId = g_debugId++;
}

ClipWorkflow::~ClipWorkflow()
{
    delete[] m_buffer;
    delete[] m_backBuffer;
    delete m_stateLock;
    delete m_requiredStateLock;
    delete m_backBufferLock;
}

unsigned char*    ClipWorkflow::getOutput()
{
    QReadLocker     lock( m_backBufferLock );
    if ( m_usingBackBuffer == true )
        return m_buffer;
    return m_backBuffer;
}

void    ClipWorkflow::checkStateChange()
{
    QMutexLocker    lock( m_requiredStateLock );
    QWriteLocker    lock2( m_stateLock );
    if ( m_requiredState != ClipWorkflow::None )
    {
        m_state = m_requiredState;
        m_requiredState = ClipWorkflow::None;
    }
}

void    ClipWorkflow::lock( ClipWorkflow* cw, void** pp_ret )
{
    QReadLocker lock( cw->m_backBufferLock );

    if ( cw->m_usingBackBuffer )
    {
        *pp_ret = cw->m_backBuffer;
    }
    else
    {
        *pp_ret = cw->m_buffer;
    }
}

void    ClipWorkflow::unlock( ClipWorkflow* cw )
{
    cw->m_stateLock->lockForWrite();

    if ( cw->m_state == Rendering )
    {
        cw->m_state = Sleeping;
        cw->m_stateLock->unlock();

        QMutexLocker    lock( cw->m_condMutex );
        cw->m_waitCond->wait( cw->m_condMutex );
        cw->m_stateLock->lockForWrite();
        cw->m_state = Rendering;
        {
            QWriteLocker    lock2( cw->m_backBufferLock );
            cw->m_usingBackBuffer = !cw->m_usingBackBuffer;
        }
    }
    cw->m_stateLock->unlock();
    cw->checkStateChange();
}

void    ClipWorkflow::setVmem()
{
    char        buffer[32];

    m_clip->getParent()->addVolatileParam( ":no-audio", ":audio" );
    m_clip->getParent()->addVolatileParam( ":vout=vmem", ":vout=''" );
    m_clip->getParent()->getVLCMedia()->setDataCtx( this );
    m_clip->getParent()->getVLCMedia()->setLockCallback( reinterpret_cast<LibVLCpp::Media::lockCallback>( &ClipWorkflow::lock ) );
    m_clip->getParent()->getVLCMedia()->setUnlockCallback( reinterpret_cast<LibVLCpp::Media::unlockCallback>( &ClipWorkflow::unlock ) );
    m_clip->getParent()->addConstantParam( ":vmem-chroma=RV24" );

    sprintf( buffer, ":vmem-width=%i", VIDEOWIDTH );
    m_clip->getParent()->addConstantParam( buffer );

    sprintf( buffer, ":vmem-height=%i", VIDEOHEIGHT );
    m_clip->getParent()->addConstantParam( buffer );

    sprintf( buffer, "vmem-pitch=%i", VIDEOWIDTH * 3 );
    m_clip->getParent()->addConstantParam( buffer );
}

void    ClipWorkflow::initialize( LibVLCpp::MediaPlayer* mediaPlayer )
{
    setState( Initializing );
    setVmem();
    m_mediaPlayer = mediaPlayer;
    m_mediaPlayer->setMedia( m_clip->getParent()->getVLCMedia() );

    connect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( setPositionAfterPlayback() ), Qt::DirectConnection );
    connect( m_mediaPlayer, SIGNAL( endReached() ), this, SLOT( clipEndReached() ), Qt::DirectConnection );
    m_mediaPlayer->play();
    //m_clip->getParent()->flushVolatileParameters();
}

void    ClipWorkflow::setPositionAfterPlayback()
{
    disconnect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( setPositionAfterPlayback() ) );
    connect( m_mediaPlayer, SIGNAL( positionChanged() ), this, SLOT( pauseAfterPlaybackStarted() ), Qt::DirectConnection );
    m_mediaPlayer->setPosition( m_clip->getBegin() );
}

void    ClipWorkflow::pauseAfterPlaybackStarted()
{
    disconnect( m_mediaPlayer, SIGNAL( positionChanged() ), this, SLOT( pauseAfterPlaybackStarted() ) );
    disconnect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( pauseAfterPlaybackStarted() ) );

    connect( m_mediaPlayer, SIGNAL( paused() ), this, SLOT( pausedMediaPlayer() ), Qt::DirectConnection );
    m_mediaPlayer->pause();

}

void    ClipWorkflow::pausedMediaPlayer()
{
    disconnect( m_mediaPlayer, SIGNAL( paused() ), this, SLOT( pausedMediaPlayer() ) );
    setState( Ready );
}

bool    ClipWorkflow::isReady() const
{
    QReadLocker lock( m_stateLock );
    return m_state == ClipWorkflow::Ready;
}

bool    ClipWorkflow::isEndReached() const
{
    QReadLocker lock( m_stateLock );
    return m_state == ClipWorkflow::EndReached;
}

bool    ClipWorkflow::isStopped() const
{
    QReadLocker lock( m_stateLock );
    return m_state == ClipWorkflow::Stopped;
}

ClipWorkflow::State     ClipWorkflow::getState() const
{
    return m_state;
}

void    ClipWorkflow::startRender()
{
    while ( isReady() == false )
        SleepMS( 1 );
    m_mediaPlayer->play();
    setState( Rendering );
}

void    ClipWorkflow::clipEndReached()
{
    setState( EndReached );
}

const Clip*     ClipWorkflow::getClip() const
{
    return m_clip;
}

void            ClipWorkflow::stop()
{
    Q_ASSERT( m_mediaPlayer != NULL );
    m_mediaPlayer->stop();
    disconnect( m_mediaPlayer, SIGNAL( endReached() ), this, SLOT( clipEndReached() ) );
    m_mediaPlayer = NULL;
    setState( Stopped );
    QMutexLocker    lock( m_requiredStateLock );
    m_requiredState = ClipWorkflow::None;
}

void            ClipWorkflow::setPosition( float pos )
{
    m_mediaPlayer->setPosition( pos );
}

bool            ClipWorkflow::isRendering() const
{
    QReadLocker lock( m_stateLock );
    return m_state == ClipWorkflow::Rendering;
}

void            ClipWorkflow::setState( State state )
{
    QWriteLocker    lock( m_stateLock );
    m_state = state;
}

void            ClipWorkflow::queryStateChange( State newState )
{
    QMutexLocker    lock( m_requiredStateLock );
    m_requiredState = newState;
}

void            ClipWorkflow::wake()
{
    m_waitCond->wakeAll();
}

QReadWriteLock* ClipWorkflow::getStateLock()
{
    return m_stateLock;
}

void            ClipWorkflow::reinitialize()
{
    QWriteLocker    lock( m_stateLock );
    m_state = Stopped;
    queryStateChange( None );
}
