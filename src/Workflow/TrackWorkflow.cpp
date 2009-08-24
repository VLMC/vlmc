/*****************************************************************************
 * TrackWorkflow.cpp : Will query the Clip workflow for each successive clip in the track
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
#include "TrackWorkflow.h"

TrackWorkflow::TrackWorkflow( unsigned int trackId ) :
        m_trackId( trackId ),
        m_length( 0 ),
        m_forceRepositionning( false ),
        m_paused( false ),
        m_synchroneRenderBuffer( NULL )
{
    m_forceRepositionningMutex = new QMutex;
    m_clipsLock = new QReadWriteLock;
}

TrackWorkflow::~TrackWorkflow()
{
    QMap<qint64, ClipWorkflow*>::iterator       it = m_clips.begin();
    QMap<qint64, ClipWorkflow*>::iterator       end = m_clips.end();

    while ( it != end )
    {
        stopClipWorkflow( it.value() );
        delete it.value();
        it = m_clips.erase( it );
    }
    delete m_clipsLock;
    delete m_forceRepositionningMutex;
}

void    TrackWorkflow::addClip( Clip* clip, qint64 start )
{
    ClipWorkflow* cw = new ClipWorkflow( clip );
    addClip( cw, start );
}

void    TrackWorkflow::addClip( ClipWorkflow* cw, qint64 start )
{
    QWriteLocker    lock( m_clipsLock );
    connect( cw, SIGNAL( renderComplete( ClipWorkflow* ) ), this, SLOT( clipWorkflowRenderCompleted( ClipWorkflow* ) ), Qt::DirectConnection );
    connect( cw, SIGNAL( paused() ), this, SLOT( clipWorkflowPaused() ) );
    connect( cw, SIGNAL( unpaused() ), this, SLOT( clipWorkflowUnpaused() ) );
    connect( cw, SIGNAL( endReached( ClipWorkflow* ) ), this, SLOT( clipWorkflowEndReached( ClipWorkflow* ) ), Qt::QueuedConnection );
    m_clips.insert( start, cw );
    computeLength();
}

//Must be called from a thread safe method (m_clipsLock locked)
void                TrackWorkflow::computeLength()
{
    if ( m_clips.count() == 0 )
    {
        m_length = 0;
        return ;
    }
    QMap<qint64, ClipWorkflow*>::const_iterator it = m_clips.end() - 1;
    m_length = (it.key() + it.value()->getClip()->getLength() );
}

qint64              TrackWorkflow::getLength() const
{
    return m_length;
}

qint64              TrackWorkflow::getClipPosition( const QUuid& uuid ) const
{
    QMap<qint64, ClipWorkflow*>::const_iterator     it = m_clips.begin();
    QMap<qint64, ClipWorkflow*>::const_iterator     end = m_clips.end();

    while ( it != end )
    {
        if ( it.value()->getClip()->getUuid() == uuid )
            return it.key();
        ++it;
    }
    return -1;
}

void        TrackWorkflow::renderClip( ClipWorkflow* cw, qint64 currentFrame,
                                        qint64 start , bool needRepositioning )
{
    cw->getStateLock()->lockForRead();

    if ( cw->getState() == ClipWorkflow::Rendering )
    {
        //The rendering state meens... whell it means that the frame is
        //beeing rendered, so we wait.
        cw->getStateLock()->unlock();
        cw->waitForCompleteRender();
        //This way we can trigger the appropriate if just below.
        //by restoring the initial state of the function, and just pretend that
        //nothing happened.
        cw->getStateLock()->lockForRead();
    }

    //If frame has been rendered :
    if ( cw->getState() == ClipWorkflow::Sleeping )
    {
        cw->getStateLock()->unlock();

        if ( needRepositioning == true )
        {
            float   pos = ( (float)( currentFrame - start ) / (float)(cw->getClip()->getLength()) );
            pos = pos * ( cw->getClip()->getEnd() - cw->getClip()->getBegin() ) + cw->getClip()->getBegin();
            cw->setPosition( pos );
        }
        QMutexLocker    lock( cw->getSleepMutex() );
        cw->wake();
    }
    else if ( cw->getState() == ClipWorkflow::Stopped )
    {
        cw->getStateLock()->unlock();
        cw->initialize();
        cw->startRender( m_paused );
        if ( start != currentFrame ) //Clip was not started as its real begining
        {
            float   pos = ( (float)( currentFrame - start ) / (float)(cw->getClip()->getLength()) );
            pos = pos * ( cw->getClip()->getEnd() - cw->getClip()->getBegin() ) + cw->getClip()->getBegin();
            cw->setPosition( pos );
        }
    }
    else if ( cw->getState() == ClipWorkflow::Ready ||
              cw->getState() == ClipWorkflow::Initializing )
    {
        //If the state is Initializing, then the workflow will wait.
        //Otherwise, it will start directly.
        cw->getStateLock()->unlock();
        cw->startRender( false );

        if ( needRepositioning == true )
        {
            float   pos = ( (float)( currentFrame - start ) / (float)(cw->getClip()->getLength()) );
            pos = pos * ( cw->getClip()->getEnd() - cw->getClip()->getBegin() ) + cw->getClip()->getBegin();
            cw->setPosition( pos );
        }
    }
    else if ( cw->getState() == ClipWorkflow::EndReached )
    {
        cw->getStateLock()->unlock();
        //The stopClipWorkflow() method will take care of that.
    }
    else if ( cw->getState() == ClipWorkflow::Paused )
    {
        cw->getStateLock()->unlock();
        if ( needRepositioning == true )
        {
            float   pos = ( (float)( currentFrame - start ) / (float)(cw->getClip()->getLength()) );
            cw->setPosition( pos );
        }
        clipWorkflowRenderCompleted( cw );
    }
    else
    {
        cw->getStateLock()->unlock();
    }
}

void                TrackWorkflow::preloadClip( ClipWorkflow* cw )
{
    cw->getStateLock()->lockForRead();

    if ( cw->getState() == ClipWorkflow::Stopped )
    {
        cw->getStateLock()->unlock();
        cw->initialize();
        return ;
    }
    cw->getStateLock()->unlock();
}

void                TrackWorkflow::stopClipWorkflow( ClipWorkflow* cw )
{
    cw->getStateLock()->lockForRead();

    if ( cw->getState() == ClipWorkflow::Stopped )
    {
        cw->getStateLock()->unlock();
        return ;
    }
    if ( cw->getState() == ClipWorkflow::Sleeping ||
         cw->getState() == ClipWorkflow::Ready ||
         cw->getState() == ClipWorkflow::EndReached )
    {
        cw->getStateLock()->unlock();
        cw->queryStateChange( ClipWorkflow::Stopping );
        cw->wake();
        cw->stop();
    }
    else if ( cw->getState() == ClipWorkflow::Rendering )
    {
        cw->getStateLock()->unlock();
        cw->waitForCompleteRender();
        {
            QMutexLocker    lock( cw->getSleepMutex() );
            cw->queryStateChange( ClipWorkflow::Stopping );
        }
        cw->wake();
        cw->stop();
    }
    else if ( cw->getState() == ClipWorkflow::Initializing )
    {
        cw->getStateLock()->unlock();
        cw->waitForCompleteInit();
        cw->stop();
    }
    else if ( cw->getState() == ClipWorkflow::Paused )
    {
        cw->getStateLock()->unlock();
        cw->queryStateChange( ClipWorkflow::Stopping );
        cw->wake();
        cw->stop();
    }
    else
    {
        qDebug() << "Unexpected ClipWorkflow::State when stopping :" << cw->getState();
        cw->getStateLock()->unlock();
        return ;
    }
}

void                    TrackWorkflow::stop()
{
    QMap<qint64, ClipWorkflow*>::iterator       it = m_clips.begin();
    QMap<qint64, ClipWorkflow*>::iterator       end = m_clips.end();

    while ( it != end )
    {
        stopClipWorkflow( it.value() );
        ++it;
    }
}

bool                TrackWorkflow::getOutput( qint64 currentFrame )
{
    QReadLocker     lock( m_clipsLock );

    QMap<qint64, ClipWorkflow*>::iterator       it = m_clips.begin();
    QMap<qint64, ClipWorkflow*>::iterator       end = m_clips.end();
    static  qint64                              lastFrame = 0;
    bool                                        needRepositioning;
    bool                                        hasRendered = false;

    {
        QMutexLocker    lock( m_forceRepositionningMutex );
        if ( m_forceRepositionning == true )
        {
            needRepositioning = true;
            m_forceRepositionning = false;
        }
        else if ( m_paused == true && currentFrame != lastFrame )
            needRepositioning = true;
        else
            needRepositioning = ( abs( currentFrame - lastFrame ) > 1 ) ? true : false;
    }
    m_nbClipToRender = 0;

    while ( it != end )
    {
        qint64          start = it.key();
        ClipWorkflow*   cw = it.value();
        //Is the clip supposed to render now ?
        if ( needRepositioning == true && it.value().activated() == false )
        {
            qDebug() << "Reactivating clipworkflow";
            it.value().activate();
        }
        if ( start <= currentFrame && currentFrame <= start + cw->getClip()->getLength() )
        {
            m_nbClipToRender.fetchAndAddAcquire( 1 );
            renderClip( cw, currentFrame, start, needRepositioning );
            lastFrame = currentFrame;
            hasRendered = true;
        }
        //Is it about to be rendered ?
        else if ( start > currentFrame &&
                start - currentFrame < TrackWorkflow::nbFrameBeforePreload )
        {
            preloadClip( cw );
        }
        //Is it supposed to be stopped ?
        else
        {
            stopClipWorkflow( cw );
        }

        ++it;
    }
    if ( hasRendered == false )
    {
        clipWorkflowRenderCompleted( NULL );
    }
    return hasRendered;
}

void                TrackWorkflow::pause()
{
    QReadLocker     lock( m_clipsLock );

    QMap<qint64, ClipWorkflow*>::iterator       it = m_clips.begin();
    QMap<qint64, ClipWorkflow*>::iterator       end = m_clips.end();
    bool                                        pauseRequired = false;

    m_nbClipToPause = 0;
    for ( ; it != end; ++it )
    {
        ClipWorkflow*   cw = it.value();

        cw->getStateLock()->lockForRead();
        if ( cw->getState() == ClipWorkflow::Stopped )
        {
            cw->getStateLock()->unlock();
            continue ;
        }
        else if ( cw->getState() != ClipWorkflow::Paused )
        {
            cw->getStateLock()->unlock();
            m_nbClipToPause.fetchAndAddAcquire( 1 );
            cw->pause();
            pauseRequired = true;
        }
        else
        {
            //This should never be used.
            qDebug() << "Asking to pause in an already paused state";
            cw->getStateLock()->unlock();
        }
    }
    if ( pauseRequired == false )
    {
        clipWorkflowPaused();
    }
}

void            TrackWorkflow::moveClip( const QUuid& id, qint64 startingFrame )
{
    QWriteLocker    lock( m_clipsLock );

    QMap<qint64, ClipWorkflow*>::iterator       it = m_clips.begin();
    QMap<qint64, ClipWorkflow*>::iterator       end = m_clips.end();

    while ( it != end )
    {
        if ( it.value()->getClip()->getUuid() == id )
        {
            ClipWorkflow* cw = it.value();
            m_clips.erase( it );
            m_clips[startingFrame] = cw;
            QMutexLocker    lock( m_forceRepositionningMutex );
            m_forceRepositionning = true;
            computeLength();
            return ;
        }
        ++it;
    }
    qDebug() << "Track" << m_trackId << "was asked to move clip" << id << "to position" << startingFrame
            << "but this clip doesn't exist in this track";
}

Clip*       TrackWorkflow::removeClip( const QUuid& id )
{
    QWriteLocker    lock( m_clipsLock );

    QMap<qint64, ClipWorkflow*>::iterator       it = m_clips.begin();
    QMap<qint64, ClipWorkflow*>::iterator       end = m_clips.end();

    while ( it != end )
    {
        if ( it.value()->getClip()->getUuid() == id )
        {
            ClipWorkflow*   cw = it.value();
            Clip*           clip = cw->getClip();
            m_clips.erase( it );
            computeLength();
            return clip;
        }
        ++it;
    }
    return NULL;
}

void        TrackWorkflow::clipWorkflowRenderCompleted( ClipWorkflow* cw )
{
    if ( cw != NULL )
    {
        m_synchroneRenderBuffer = cw->getOutput();
    }
    else
    {
        m_synchroneRenderBuffer = NULL;
    }
    m_nbClipToRender.fetchAndAddAcquire( -1 );
    //When there is nothing to render, m_nbClipToRender will be equal to one here, so we check for minus
    //or equal to 0
    if ( m_nbClipToRender <= 0 )
    {
        emit renderCompleted( m_trackId );
    }
}

unsigned char*  TrackWorkflow::getSynchroneOutput()
{
    return m_synchroneRenderBuffer;
}

void    TrackWorkflow::clipWorkflowPaused()
{
    m_nbClipToPause.fetchAndAddAcquire( -1 );
    if ( m_nbClipToPause <= 0 )
    {
        m_paused = true;
        emit trackPaused();
    }
}

void    TrackWorkflow::unpause()
{
    QReadLocker     lock( m_clipsLock );

    QMap<qint64, ClipWorkflow*>::iterator       it = m_clips.begin();
    QMap<qint64, ClipWorkflow*>::iterator       end = m_clips.end();
    bool                                        unpauseRequired = false;

    m_nbClipToUnpause = 0;
    for ( ; it != end; ++it )
    {
        ClipWorkflow*   cw = it.value();

        cw->getStateLock()->lockForRead();
        if ( cw->getState() == ClipWorkflow::Paused )
        {
            cw->getStateLock()->unlock();
            m_nbClipToUnpause.fetchAndAddAcquire( 1 );
            cw->unpause();
            unpauseRequired = true;
        }
        else
        {
            cw->getStateLock()->unlock();
        }
    }
    if ( unpauseRequired == false )
        clipWorkflowUnpaused();
}

void    TrackWorkflow::clipWorkflowUnpaused()
{
    m_nbClipToUnpause.fetchAndAddAcquire( -1 );
    if ( m_nbClipToUnpause <= 0 )
    {
        m_paused = false;
        emit trackUnpaused();
    }
}

void    TrackWorkflow::clipWorkflowEndReached( ClipWorkflow* cw )
{
    //If this clip is the last, we emit the trackEndReached signal, and stop the
    //clip workflow.
    cw->stop();

    //this obviously couldn't happen, since we have a clipworkflow...
    Q_ASSERT ( m_clips.size() != 0 );

    QMap<qint64, ClipWorkflow*>::const_iterator   it = m_clips.end() - 1;
    //If it ends before the current frame, we reached end.
    if ( it.value() == cw )
        emit trackEndReached( m_trackId );
    //Else: simulating that the clip render is complete, because the TrackWorkflow
    //waits for this clip to render.       
}
