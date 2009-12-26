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
#include "MemoryPool.hpp"
#include "LightVideoFrame.h"

ClipWorkflow::ClipWorkflow( Clip::Clip* clip ) :
                m_mediaPlayer(NULL),
                m_requiredState( ClipWorkflow::None ),
                m_rendering( false ),
                m_initFlag( false ),
                m_clip( clip ),
                m_state( ClipWorkflow::Stopped )
{
    m_stateLock = new QReadWriteLock;
    m_requiredStateLock = new QMutex;
    m_initWaitCond = new WaitCondition;
    m_pausingStateWaitCond = new WaitCondition;
    m_renderLock = new QMutex;
    m_feedingCondWait = new WaitCondition;
}

ClipWorkflow::~ClipWorkflow()
{
    delete m_renderLock;
    delete m_pausingStateWaitCond;
    delete m_initWaitCond;
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
    }
}

void    ClipWorkflow::initialize()
{
    if ( m_clip->getParent()->getFileType() == Media::Image )
        m_vlcMedia = new LibVLCpp::Media( "fake:///" + m_clip->getParent()->getFileInfo()->absoluteFilePath() );
    else
        m_vlcMedia = new LibVLCpp::Media( "file:///" + m_clip->getParent()->getFileInfo()->absoluteFilePath() );
    m_currentPts = -1;
    m_previousPts = -1;
    initVlcOutput();
    m_mediaPlayer = MemoryPool<LibVLCpp::MediaPlayer>::getInstance()->get();
    m_mediaPlayer->setMedia( m_vlcMedia );

    connect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( loadingComplete() ), Qt::DirectConnection );
    connect( m_mediaPlayer, SIGNAL( endReached() ), this, SLOT( clipEndReached() ), Qt::DirectConnection );
    qDebug() << "Starting clipworkflow playback";
    m_mediaPlayer->play();
}

void    ClipWorkflow::loadingComplete()
{
    adjustBegin();
    disconnect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( loadingComplete() ) );
    qDebug() << "Setting state to Rendering";
    QMutexLocker    lock( m_initWaitCond->getMutex() );
    setState( Rendering );
    qDebug() << "Waking init wait cond";
    m_initWaitCond->wake();
}

void    ClipWorkflow::adjustBegin()
{
    if ( m_clip->getParent()->getFileType() == Media::Video || m_clip->getParent()->getFileType() == Media::Audio )
        m_mediaPlayer->setTime( m_clip->getBegin() / m_clip->getParent()->getFps() * 1000 );
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
        disconnect( m_mediaPlayer, SIGNAL( endReached() ), this, SLOT( clipEndReached() ) );
        MemoryPool<LibVLCpp::MediaPlayer>::getInstance()->release( m_mediaPlayer );
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

void            ClipWorkflow::setState( State state )
{
    QWriteLocker    lock( m_stateLock );
//        qDebug() << '[' << (void*)this << "] Setting state to" << state;
    m_state = state;
}

void            ClipWorkflow::queryStateChange( State newState )
{
    QMutexLocker    lock( m_requiredStateLock );
    m_requiredState = newState;
}

QReadWriteLock* ClipWorkflow::getStateLock()
{
    return m_stateLock;
}

void        ClipWorkflow::waitForCompleteInit()
{
    if ( isRendering() == false )
    {
        QMutexLocker    lock( m_initWaitCond->getMutex() );
        m_initWaitCond->waitLocked();
    }
}

LibVLCpp::MediaPlayer*       ClipWorkflow::getMediaPlayer()
{
    return m_mediaPlayer;
}

void        ClipWorkflow::postGetOutput()
{
    //If we're running out of computed buffers, refill our stack.
    if ( getComputedBuffers() < getMaxComputedBuffers() / 3 )
    {
        if ( m_mediaPlayer->isPlaying() == false )
        {
            qWarning() << "Unpausing media player";
            m_mediaPlayer->pause();
        }
        else
            qCritical() << "Running out of output buffers !";
    }
}

void        ClipWorkflow::preGetOutput()
{
    QMutexLocker    lock( m_feedingCondWait->getMutex() );

    if ( getComputedBuffers() == 0 )
    {
        qWarning() << "Waiting for buffer to be fed";
        m_renderLock->unlock();
        m_feedingCondWait->waitLocked();
        m_renderLock->lock();
    }
}

void        ClipWorkflow::commonUnlock()
{
    if ( getAvailableBuffers() == 0 )
    {
        qDebug() << "No more available buffers : pausing";
        m_mediaPlayer->pause();
    }
    if ( getComputedBuffers() == 1 )
    {
        QMutexLocker    lock( m_feedingCondWait->getMutex() );
        qWarning() << "Just rendered the first buffer.";
        m_feedingCondWait->wake();
    }
    checkStateChange();
}

void    ClipWorkflow::computePtsDiff( qint64 pts )
{
    if ( m_previousPts == -1 )
        m_previousPts = pts;
    if ( m_currentPts == -1 )
        m_currentPts = pts;
    m_previousPts = m_currentPts;
    m_currentPts = qMax( pts, m_previousPts );
}
