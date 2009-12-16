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
#include "LightVideoFrame.h"

ClipWorkflow::ClipWorkflow( Clip::Clip* clip ) :
                m_mediaPlayer(NULL),
                m_requiredState( ClipWorkflow::None ),
                m_rendering( false ),
                m_initFlag( false ),
                m_clip( clip ),
                m_state( ClipWorkflow::Stopped ),
                m_fullSpeedRender( false )
{
    m_stateLock = new QReadWriteLock;
    m_requiredStateLock = new QMutex;
    m_waitCond = new QWaitCondition;
    m_condMutex = new QMutex;
    m_initWaitCond = new WaitCondition;
    m_renderWaitCond = new WaitCondition;
    m_pausingStateWaitCond = new WaitCondition;
    m_renderLock = new QMutex;
}

ClipWorkflow::~ClipWorkflow()
{
    delete m_renderLock;
    delete m_pausingStateWaitCond;
    delete m_initWaitCond;
    delete m_condMutex;
    delete m_waitCond;
    delete m_renderWaitCond;
    delete m_requiredStateLock;
    delete m_stateLock;
}

void    ClipWorkflow::checkStateChange()
{
    QMutexLocker    lock( m_requiredStateLock );
    QWriteLocker    lock2( m_stateLock );
    if ( m_requiredState != ClipWorkflow::None )
    {
        m_state = m_requiredState;
//        qDebug() << '[' << (void*)this << "] Applying required state change:" << m_state;
        m_requiredState = ClipWorkflow::None;
        checkSynchronisation( m_state );
    }
}

void    ClipWorkflow::initialize( bool preloading /*= false*/ )
{
//    qDebug() << "Setting state to initializing";
    setState( Initializing );
//    qDebug() << "State is Initializing.";
    if ( m_clip->getParent()->getFileType() == Media::Image )
        m_vlcMedia = new LibVLCpp::Media( "fake:///" + m_clip->getParent()->getFileInfo()->absoluteFilePath() );
    else
        m_vlcMedia = new LibVLCpp::Media( "file:///" + m_clip->getParent()->getFileInfo()->absoluteFilePath() );
    initVlcOutput();
    m_mediaPlayer = Pool<LibVLCpp::MediaPlayer>::getInstance()->get();
    m_mediaPlayer->setMedia( m_vlcMedia );

    if ( preloading == true )
        connect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( pauseAfterPlaybackStarted() ), Qt::DirectConnection );
    else
        connect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( loadingComplete() ), Qt::DirectConnection );
    connect( m_mediaPlayer, SIGNAL( endReached() ), this, SLOT( clipEndReached() ), Qt::DirectConnection );
    m_mediaPlayer->play();
}

void    ClipWorkflow::pauseAfterPlaybackStarted()
{
    adjustBegin();
    disconnect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( pauseAfterPlaybackStarted() ) );
    connect( m_mediaPlayer, SIGNAL( paused() ), this, SLOT( initializedMediaPlayer() ), Qt::DirectConnection );

    m_mediaPlayer->pause();
}

void    ClipWorkflow::loadingComplete()
{
//    qDebug() << "Loading complete, setting begin.";
    adjustBegin();
    disconnect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( loadingComplete() ) );
    setState( Ready );
//    qDebug() << "State is Ready";
}

void    ClipWorkflow::initializedMediaPlayer()
{
    disconnect( m_mediaPlayer, SIGNAL( paused() ), this, SLOT( initializedMediaPlayer() ) );
    setState( Ready );
}

void    ClipWorkflow::adjustBegin()
{
    if ( m_clip->getParent()->getFileType() == Media::Video || m_clip->getParent()->getFileType() == Media::Audio )
        m_mediaPlayer->setTime( m_clip->getBegin() / m_clip->getParent()->getFps() * 1000 );
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
    {
        QMutexLocker    lock( m_initWaitCond->getMutex() );
        if ( isReady() == false )
        {
    //        qDebug() << "Waiting for clipworkflow to be ready";
            m_initWaitCond->waitLocked();
        }
    }
//    qDebug() << "ClipWorkflow is ready";
    if ( startInPausedMode == false )
    {
        m_mediaPlayer->play();
//        qDebug() << "Setting state: Rendering";
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
        disconnect( m_mediaPlayer, SIGNAL( endReached() ), this, SLOT( clipEndReached() ) );
        Pool<LibVLCpp::MediaPlayer>::getInstance()->release( m_mediaPlayer );
//        qDebug() << "Setting media player to NULL";
        m_mediaPlayer = NULL;
        setState( Stopped );
        QMutexLocker    lock( m_requiredStateLock );
        m_requiredState = ClipWorkflow::None;
        delete m_vlcMedia;
        m_initFlag = false;
        m_rendering = false;
//        qDebug() << "Clipworkflow stopped";
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
//        qDebug() << '[' << (void*)this << "] Setting state to" << state;
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
        m_renderWaitCond->waitLocked();
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

void        ClipWorkflow::setFullSpeedRender( bool value )
{
    m_fullSpeedRender = value;
}

WaitCondition*  ClipWorkflow::getRenderCondWait()
{
    return m_renderWaitCond;
}
