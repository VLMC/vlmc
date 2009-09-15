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
                m_mediaPlayer(NULL),
                m_state( ClipWorkflow::Stopped ),
                m_requiredState( ClipWorkflow::None ),
                m_rendering( false ),
                m_initFlag( false )
{
    m_stateLock = new QReadWriteLock;
    m_requiredStateLock = new QMutex;
    m_waitCond = new QWaitCondition;
    m_condMutex = new QMutex;
    m_initWaitCond = new WaitCondition;
    m_renderWaitCond = new WaitCondition;
    m_pausingStateWaitCond = new WaitCondition;
    m_renderLock = new QMutex;
    m_buffer = new unsigned char[VIDEOHEIGHT * VIDEOWIDTH * 4];
}

ClipWorkflow::~ClipWorkflow()
{
    delete m_renderLock;
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
    QMutexLocker    lock( m_renderLock );

    if ( isEndReached() == true )
        return NULL;
    return m_buffer;
}

void    ClipWorkflow::checkStateChange()
{
    QMutexLocker    lock( m_requiredStateLock );
    QWriteLocker    lock2( m_stateLock );
    if ( m_requiredState != ClipWorkflow::None )
    {
        m_state = m_requiredState;
//        qDebug() << "Applying required state change:" << m_state;
        m_requiredState = ClipWorkflow::None;
        checkSynchronisation( m_state );
    }
}

void    ClipWorkflow::lock( ClipWorkflow* cw, void** pp_ret, int size )
{
    Q_UNUSED( size );
    cw->m_renderLock->lock();
    *pp_ret = cw->m_buffer;
}

void    ClipWorkflow::unlock( ClipWorkflow* cw, void* buffer, int width, int height, int bpp, int size )
{
    Q_UNUSED( buffer );
    Q_UNUSED( width );
    Q_UNUSED( height );
    Q_UNUSED( bpp );
    Q_UNUSED( size );
    cw->m_renderLock->unlock();
    cw->m_stateLock->lockForWrite();

    if ( cw->m_state == Rendering )
    {
        QMutexLocker    lock( cw->m_condMutex );

        cw->m_state = Sleeping;
        cw->m_stateLock->unlock();

        cw->m_renderWaitCond->wake();
        cw->emit renderComplete( cw );

//        qDebug() << "Entering cond wait";
        cw->m_waitCond->wait( cw->m_condMutex );
//        qDebug() << "Leaving condwait";
        cw->m_stateLock->lockForWrite();
        if ( cw->m_state == Sleeping )
            cw->m_state = Rendering;
        cw->m_stateLock->unlock();
    }
    else
        cw->m_stateLock->unlock();
    cw->checkStateChange();
}

void    ClipWorkflow::setVmem()
{
    char        buffer[32];

    m_vlcMedia->addOption( ":no-audio" );
    m_vlcMedia->addOption( ":sout=#transcode{}:smem" );
    m_vlcMedia->setDataCtx( this );
    m_vlcMedia->setLockCallback( reinterpret_cast<LibVLCpp::Media::lockCallback>( &ClipWorkflow::lock ) );
    m_vlcMedia->setUnlockCallback( reinterpret_cast<LibVLCpp::Media::unlockCallback>( &ClipWorkflow::unlock ) );
    m_vlcMedia->addOption( ":sout-transcode-vcodec=RV24" );
    m_vlcMedia->addOption( ":sout-transcode-acodec=s16l" );

    sprintf( buffer, ":sout-transcode-width=%i", VIDEOWIDTH );
    m_vlcMedia->addOption( buffer );

    sprintf( buffer, ":sout-transcode-height=%i", VIDEOHEIGHT );
    m_vlcMedia->addOption( buffer );

    //sprintf( buffer, "sout-smem-video-pitch=%i", VIDEOWIDTH * 3 );
    //m_vlcMedia->addOption( buffer );
}

void    ClipWorkflow::initialize()
{
    setState( Initializing );
    m_vlcMedia = new LibVLCpp::Media( "file://" + m_clip->getParent()->getFileInfo()->absoluteFilePath() );
    setVmem();
    m_mediaPlayer = Pool<LibVLCpp::MediaPlayer>::getInstance()->get();
    m_mediaPlayer->setMedia( m_vlcMedia );

    connect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( setPositionAfterPlayback() ), Qt::DirectConnection );
    connect( m_mediaPlayer, SIGNAL( endReached() ), this, SLOT( clipEndReached() ), Qt::DirectConnection );
    m_mediaPlayer->play();
}

//FIXME: this step is probably useless, due to modification in the TrackWorkflow
void    ClipWorkflow::setPositionAfterPlayback()
{
    disconnect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( setPositionAfterPlayback() ) );
    connect( m_mediaPlayer, SIGNAL( positionChanged() ), this, SLOT( pauseAfterPlaybackStarted() ), Qt::DirectConnection );

    m_mediaPlayer->setPosition( m_clip->getBegin() );
}

void    ClipWorkflow::pauseAfterPlaybackStarted()
{
    disconnect( m_mediaPlayer, SIGNAL( positionChanged() ), this, SLOT( pauseAfterPlaybackStarted() ) );
    //FIXME: it seems that this signal is never connected :o
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

void    ClipWorkflow::startRender( bool startInPausedMode )
{
    if ( isReady() == false )
    {
        QMutexLocker    lock( m_initWaitCond->getMutex() );
        m_initWaitCond->waitLocked();
    }

    if ( startInPausedMode == false )
    {
        m_mediaPlayer->play();
        setState( Rendering );
    }
    else
    {
        setState( Paused );
    }
}

void    ClipWorkflow::clipEndReached()
{
    setState( EndReached );
    emit renderComplete( this );
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
        m_initFlag = false;
        m_rendering = false;
    }
    else
        qDebug() << "ClipWorkflow has already been stopped";
}

void            ClipWorkflow::setTime( qint64 time )
{
    m_mediaPlayer->setTime( time );
}

bool            ClipWorkflow::isRendering() const
{
    QReadLocker lock( m_stateLock );
    return m_state == ClipWorkflow::Rendering;
}

bool            ClipWorkflow::isSleeping() const
{
    QReadLocker lock( m_stateLock );
    return m_state == ClipWorkflow::Sleeping;
}

void            ClipWorkflow::checkSynchronisation( State newState )
{
    switch ( newState )
    {
        case Ready:
        {
            QMutexLocker    lock( m_initWaitCond->getMutex() );
            m_initWaitCond->wake();
            break ;
        }
        default:
            break ;
    }
}

void            ClipWorkflow::setState( State state )
{
    {
        QWriteLocker    lock( m_stateLock );
//        qDebug() << "Setting state to" << state;
        m_state = state;
    }
    checkSynchronisation( state );
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

void            ClipWorkflow::pause()
{
    connect( m_mediaPlayer, SIGNAL( paused() ), this, SLOT( pausedMediaPlayer() ), Qt::DirectConnection );
    setState( Pausing );
    m_mediaPlayer->pause();
    QMutexLocker    lock( m_requiredStateLock );
    m_requiredState = ClipWorkflow::None;

    {
        QMutexLocker    sync( m_condMutex );
        wake();
    }
}

void            ClipWorkflow::unpause()
{
    queryStateChange( ClipWorkflow::Rendering );
    connect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( unpausedMediaPlayer() ), Qt::DirectConnection );
    m_mediaPlayer->pause();
}

void        ClipWorkflow::waitForCompleteRender( bool dontCheckRenderStarted /*= false*/ )
{
    if ( isRendering() == true || dontCheckRenderStarted == true )
        m_renderWaitCond->wait();
}

void        ClipWorkflow::waitForCompleteInit()
{
    if ( isReady() == false )
    {
        QMutexLocker    lock( m_initWaitCond->getMutex() );
        m_initWaitCond->waitLocked();
    }
}

QMutex*     ClipWorkflow::getSleepMutex()
{
    return m_condMutex;
}

LibVLCpp::MediaPlayer*       ClipWorkflow::getMediaPlayer()
{
    return m_mediaPlayer;
}

void        ClipWorkflow::pausedMediaPlayer()
{
    disconnect( m_mediaPlayer, SIGNAL( paused() ), this, SLOT( pausedMediaPlayer() ) );
    setState( Paused );
    emit paused();
}

void        ClipWorkflow::unpausedMediaPlayer()
{
    disconnect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( unpausedMediaPlayer() ) );
    emit unpaused();
}
