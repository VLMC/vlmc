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
#include "Pool.hpp"

ClipWorkflow::ClipWorkflow( Clip::Clip* clip ) :
                m_clip( clip ),
                m_buffer( NULL ),
                //m_usingBackBuffer( false ),
                m_mediaPlayer(NULL),
                m_state( ClipWorkflow::Stopped ),
                m_requiredState( ClipWorkflow::None )
{
    m_buffer = new unsigned char[VIDEOHEIGHT * VIDEOWIDTH * 4];
//    m_backBuffer = new unsigned char[VIDEOHEIGHT * VIDEOWIDTH * 4];
    m_stateLock = new QReadWriteLock;
    m_requiredStateLock = new QMutex;
    m_waitCond = new QWaitCondition;
    m_condMutex = new QMutex;
    m_initWaitCond = new WaitCondition;
    m_renderWaitCond = new WaitCondition;
    m_pausingStateWaitCond = new WaitCondition;
    m_pausedThreadCondWait = new WaitCondition;
//    m_backBufferLock = new QReadWriteLock;
}

ClipWorkflow::~ClipWorkflow()
{
//    delete[] m_backBuffer;
//    delete m_backBufferLock;
    delete m_pausedThreadCondWait;
    delete m_pausingStateWaitCond;
    delete m_initWaitCond;
    delete m_condMutex;
    delete m_waitCond;
    delete m_requiredStateLock;
    delete m_stateLock;
    delete[] m_buffer;
}

unsigned char*    ClipWorkflow::getOutput()
{
//    QReadLocker     lock( m_backBufferLock );
//    if ( m_usingBackBuffer == true )
        return m_buffer;
//    return m_backBuffer;
}

void    ClipWorkflow::checkStateChange()
{
    QMutexLocker    lock( m_requiredStateLock );
    QWriteLocker    lock2( m_stateLock );
    if ( m_requiredState != ClipWorkflow::None )
    {
        qDebug() << "Changed state from" << m_state << "to state" << m_requiredState;
        m_state = m_requiredState;
        m_requiredState = ClipWorkflow::None;
        checkSynchronisation( m_state );
    }
}

void    ClipWorkflow::lock( ClipWorkflow* cw, void** pp_ret )
{
//    QReadLocker lock( cw->m_backBufferLock );

//    if ( cw->m_usingBackBuffer )
//    {
//        *pp_ret = cw->m_backBuffer;
//    }
//    else
//    {
        *pp_ret = cw->m_buffer;
        qDebug() << "Clip workflow locking <<<<<<<<<<<<<<<<<<<<<<<<<<";
//    }
}

void    ClipWorkflow::unlock( ClipWorkflow* cw )
{
    qDebug() << "ClipWorkflow::unlock()";
    cw->m_stateLock->lockForWrite();

//    if ( cw->m_oneFrameOnly )
//    {
//        qDebug() << "One frame only mode is ON :)";
//        //Forcing pause after rendering a frame
//        cw->m_oneFrameOnly = 0;
//        cw->m_state = Paused;
//    }
//    else
//        qDebug() << "One frame only mode is OFF :(";
    if ( cw->m_state == Rendering )
    {
        QMutexLocker    lock( cw->m_condMutex );

        cw->m_state = Sleeping;
        cw->m_stateLock->unlock();
        //Signal that render has been completed.
        cw->m_renderWaitCond->wake();
        qDebug() << "Clip render completed";
        cw->emit renderComplete( cw );

//        qDebug() << "Entering condwait";
        cw->m_waitCond->wait( cw->m_condMutex );
//        qDebug() << "Leaved condwait";
        cw->m_stateLock->lockForWrite();
        cw->m_state = Rendering;
//        {
//            QWriteLocker    lock2( cw->m_backBufferLock );
//            cw->m_usingBackBuffer = !cw->m_usingBackBuffer;
//        }
        cw->m_stateLock->unlock();
    }
    else if ( cw->m_state == Paused )
    {
        QMutexLocker    lock( cw->m_condMutex );

        cw->m_stateLock->unlock();
        qDebug() << "Entering forced pause condwait";
        cw->setState( ClipWorkflow::ThreadPaused );
        cw->m_pausedThreadCondWait->wake();
        cw->m_waitCond->wait( cw->m_condMutex );
        qDebug() << "Leaving forced pause condwait";
        cw->setState( ClipWorkflow::Paused );
    }
    else
        cw->m_stateLock->unlock();
    qDebug() << "End of the ClipWorkflow::unlock()";
    cw->checkStateChange();
}

void    ClipWorkflow::setVmem()
{
    char        buffer[32];

    m_vlcMedia->addOption( ":no-audio" );
    m_vlcMedia->addOption( ":vout=vmem" );
    m_vlcMedia->setDataCtx( this );
    m_vlcMedia->setLockCallback( reinterpret_cast<LibVLCpp::Media::lockCallback>( &ClipWorkflow::lock ) );
    m_vlcMedia->setUnlockCallback( reinterpret_cast<LibVLCpp::Media::unlockCallback>( &ClipWorkflow::unlock ) );
    m_vlcMedia->addOption( ":vmem-chroma=RV24" );

    sprintf( buffer, ":vmem-width=%i", VIDEOWIDTH );
    m_vlcMedia->addOption( buffer );

    sprintf( buffer, ":vmem-height=%i", VIDEOHEIGHT );
    m_vlcMedia->addOption( buffer );

    sprintf( buffer, "vmem-pitch=%i", VIDEOWIDTH * 3 );
    m_vlcMedia->addOption( buffer );
}

void    ClipWorkflow::initialize()
{
    setState( Initializing );
    m_vlcMedia = new LibVLCpp::Media( m_clip->getParent()->getFileInfo()->absoluteFilePath() );
    setVmem();
    m_mediaPlayer = Pool<LibVLCpp::MediaPlayer>::getInstance()->get();
    m_mediaPlayer->setMedia( m_vlcMedia );

    connect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( setPositionAfterPlayback() ), Qt::DirectConnection );
    connect( m_mediaPlayer, SIGNAL( endReached() ), this, SLOT( clipEndReached() ), Qt::DirectConnection );
    m_mediaPlayer->play();
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

    connect( m_mediaPlayer, SIGNAL( paused() ), this, SLOT( initializedMediaPlayer() ), Qt::DirectConnection );
    m_mediaPlayer->pause();

}

void    ClipWorkflow::initializedMediaPlayer()
{
    disconnect( m_mediaPlayer, SIGNAL( paused() ), this, SLOT( initializedMediaPlayer() ) );
    setState( Ready );
}

bool    ClipWorkflow::isReady() const
{
    QReadLocker lock( m_stateLock );
    return m_state == ClipWorkflow::Ready;
}

bool    ClipWorkflow::isPausing() const
{
    QReadLocker lock( m_stateLock );
    return m_state == ClipWorkflow::Pausing;
}

bool    ClipWorkflow::isThreadPaused() const
{
    QReadLocker lock( m_stateLock );
    return m_state == ClipWorkflow::ThreadPaused;
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
    if ( isReady() == false )
        m_initWaitCond->wait();

    m_mediaPlayer->play();
    setState( Rendering );
}

void    ClipWorkflow::clipEndReached()
{
    setState( EndReached );
}

Clip*     ClipWorkflow::getClip()
{
    return m_clip;
}

void            ClipWorkflow::stop()
{
    if ( m_mediaPlayer )
    {
        m_mediaPlayer->stop();
        Pool<LibVLCpp::MediaPlayer>::getInstance()->release( m_mediaPlayer );
        disconnect( m_mediaPlayer, SIGNAL( endReached() ), this, SLOT( clipEndReached() ) );
        m_mediaPlayer = NULL;
        setState( Stopped );
        QMutexLocker    lock( m_requiredStateLock );
        m_requiredState = ClipWorkflow::None;
        delete m_vlcMedia;
    }
    else
        qDebug() << "ClipWorkflow has already been stopped";
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

void            ClipWorkflow::checkSynchronisation( State newState )
{
    switch ( newState )
    {
        case Ready:
            m_initWaitCond->wake();
            break ;
        case Pausing:
            m_pausingStateWaitCond->wake();
            break ;
        default:
            break ;
    }
}

void            ClipWorkflow::setState( State state )
{
    {
        QWriteLocker    lock( m_stateLock );
        m_state = state;
    }
    checkSynchronisation( state );
}

void            ClipWorkflow::queryStateChange( State newState )
{
//    qDebug() << "Querying state change to" << newState;
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

void            ClipWorkflow::pause()
{
    setState( Paused );
    m_mediaPlayer->pause();
    QMutexLocker    lock( m_requiredStateLock );
    m_requiredState = ClipWorkflow::None;
}

void            ClipWorkflow::unpause( bool wakeRenderThread /*= true*/ )
{
    //Since VLC will detect that the media player is paused and unpause it, we can do this safely
    setState( ClipWorkflow::Rendering );
    m_mediaPlayer->pause();
//    QMutexLocker    lock( m_requiredStateLock );
//    m_requiredState = ClipWorkflow::None;
    if ( wakeRenderThread == true )
    {
        qDebug() << "Unpausing and waking thread";
        wake();
    }
}

//void            ClipWorkflow::activateOneFrameOnly()
//{
//    qDebug() << "Activating one frame only";
//    m_oneFrameOnly = 1;
//}

void        ClipWorkflow::waitForCompleteRender( bool dontCheckRenderStarted /*= false*/ )
{
    if ( isRendering() == true || dontCheckRenderStarted == true )
        m_renderWaitCond->wait();
    else
        qDebug() << "waitForCompleteRender(), not rendering. State == " << getState();
}

void        ClipWorkflow::waitForCompleteInit()
{
    if ( isReady() == false )
        m_initWaitCond->wait();
}

void        ClipWorkflow::waitForPausingState()
{
    if ( isPausing() == false )
        m_pausingStateWaitCond->wait();
}

void        ClipWorkflow::waitForPausedThread()
{
    if ( isThreadPaused() == false )
        m_pausedThreadCondWait->wait();
}

QMutex*     ClipWorkflow::getSleepMutex()
{
    return m_condMutex;
}

LibVLCpp::MediaPlayer*       ClipWorkflow::getMediaPlayer()
{
    return m_mediaPlayer;
}
