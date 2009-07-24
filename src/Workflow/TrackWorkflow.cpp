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
        m_paused( false )
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

unsigned char*      TrackWorkflow::renderClip( ClipWorkflow* cw, qint64 currentFrame,
                                        qint64 start , bool needRepositioning,
                                        bool pauseAfterRender )
{
    unsigned char*      ret = NULL;

    cw->getStateLock()->lockForRead();

//    qDebug() << "Rendering clip";
    if ( cw->getState() == ClipWorkflow::Paused && pauseAfterRender == false )
    {
        cw->getStateLock()->unlock();
//        qDebug() << "Unpausing clip workflow";
        //If we must pause after render, we must NOT wake the renderer thread, or it could render more than one frame
        // (since this is for the next/previous frame)
        //However, if this is just for a classic unpause, with just don't give a shit :)
//        qDebug() << "Unpausing clip";
        cw->unpause( true );
        cw->getStateLock()->lockForRead();
    }
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
    if ( cw->getState() == ClipWorkflow::Sleeping || pauseAfterRender == true )
    {
        cw->getStateLock()->unlock();
        if ( needRepositioning == true )
        {
            float   pos = ( (float)( currentFrame - start ) / (float)(cw->getClip()->getLength()) );
            cw->setPosition( pos );
        }
        ret = cw->getOutput();
        if ( pauseAfterRender == true )
        {
            cw->unpause( false );
//            qDebug() << "Querying state back to pause after render";
            cw->queryStateChange( ClipWorkflow::Paused );
        }
        qDebug() << ">>> Awaking ClipWorkflow thread for render";
        cw->wake();
    }
    else if ( cw->getState() == ClipWorkflow::Stopped )
    {
        cw->getStateLock()->unlock();
        cw->initialize( );
        cw->startRender();
        if ( start != currentFrame ) //Clip was not started as its real begining
        {
            float   pos = ( (float)( currentFrame - start ) / (float)(cw->getClip()->getLength()) );
            cw->setPosition( pos );
        }
    }
    else if ( cw->getState() == ClipWorkflow::Ready ||
              cw->getState() == ClipWorkflow::Initializing )
    {
        //If the state is Initializing, then the workflow will wait.
        //Otherwise, it will start directly.
        cw->getStateLock()->unlock();
        cw->startRender();
        if ( needRepositioning == true )
        {
            float   pos = ( (float)( currentFrame - start ) / (float)(cw->getClip()->getLength()) );
            cw->setPosition( pos );
        }
    }
    else if ( cw->getState() == ClipWorkflow::EndReached )
    {
        cw->getStateLock()->unlock();
        //The stopClipWorkflow() method will take care of that.
    }
    else
    {
//        qDebug() << "Unexpected ClipWorkflow::State when rendering:" << cw->getState();
        cw->getStateLock()->unlock();
    }
    return ret;
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
        qDebug() << "Stopping while rendering";
        cw->getStateLock()->unlock();
        qDebug() << "Waiting complete render";
        cw->waitForCompleteRender();
        qDebug() << "Waited ok.";
        cw->queryStateChange( ClipWorkflow::Stopping );
        cw->wake();
        cw->stop();
        qDebug() << "Stopped clip workflow";
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
        cw->unpause();
        cw->stop();
    }
    else
    {
        qDebug() << "Unexpected ClipWorkflow::State when stopping :" << cw->getState();
        cw->getStateLock()->unlock();
    }
}

bool                TrackWorkflow::checkEnd( qint64 currentFrame ) const
{
    if ( m_clips.size() == 0 )
        return true;
    //This is the last video by chronological order :
    QMap<qint64, ClipWorkflow*>::const_iterator   it = m_clips.end() - 1;
    //If it ends before the current frame, we reached end.
    return ( it.value()->getClip()->getLength() + it.key() < currentFrame );
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

unsigned char*      TrackWorkflow::getOutput( qint64 currentFrame )
{
    QReadLocker     lock( m_clipsLock );

    unsigned char*  ret = NULL;
    QMap<qint64, ClipWorkflow*>::iterator       it = m_clips.begin();
    QMap<qint64, ClipWorkflow*>::iterator       end = m_clips.end();
    static  qint64                              lastFrame = 0;
    bool                                        needRepositioning;
    bool                                        oneFrameOnlyFlag = false;

qDebug() << "Asking for track output";
    //    qDebug() << "Checking flag...";
    if ( m_oneFrameOnly == 1 )
    {
//        qDebug() << "...Flag is activated";
        oneFrameOnlyFlag = true;
    }
//    else
//        qDebug() << "...Flag is OFF";
    if ( checkEnd( currentFrame ) == true )
    {
        emit trackEndReached( m_trackId );
        //We continue, as there can be ClipWorkflow that required to be stopped.
    }
    {
        QMutexLocker    lock( m_forceRepositionningMutex );
        if ( m_forceRepositionning == true )
        {
            needRepositioning = true;
            m_forceRepositionning = false;
        }
        else
            needRepositioning = ( abs( currentFrame - lastFrame ) > 1 ) ? true : false;
    }
    while ( it != end )
    {
        qint64          start = it.key();
        ClipWorkflow*   cw = it.value();

        //Is the clip supposed to render now ?
        if ( start <= currentFrame && currentFrame <= start + cw->getClip()->getLength() )
        {
//            if ( oneFrameOnlyFlag == true )
//                cw->activateOneFrameOnly();
            ret = renderClip( cw, currentFrame, start, needRepositioning, oneFrameOnlyFlag );
            if ( oneFrameOnlyFlag == true )
            {
                cw->pause();
//                qDebug() << "Pausing back clip workflow";
            }
            lastFrame = currentFrame;
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
    if ( oneFrameOnlyFlag == true )
    {
//        qDebug() << "Switching off m_oneFrameOnly";
        m_oneFrameOnly = 0;
    }
    return ret;
}

void            TrackWorkflow::pauseClipWorkflow( ClipWorkflow* cw )
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
        qDebug() << "Pausing a sleeping, ready or EndReached ClipWorkflow, state =" << cw->getState();
        qDebug() << "Unlocked state mutex";
        cw->getStateLock()->unlock();

        qDebug() << "Waiting for sleep mutex";
        QMutexLocker    lock( cw->getSleepMutex() );
        qDebug() << "Got sleep mutex";
        cw->queryStateChange( ClipWorkflow::Pausing );
        cw->wake();
    }
    else if ( cw->getState() == ClipWorkflow::Rendering )
    {
        qDebug() << "Pausing a rendering clip workflow";
        cw->getStateLock()->unlock();
        qDebug() << "Waiting for render complete";
        cw->waitForCompleteRender();
        qDebug() << "Waiting complete";
        qDebug() << "Waiting for sleep mutex";
        QMutexLocker    lock( cw->getSleepMutex() );
        qDebug() << "Got sleep mutex";
        cw->queryStateChange( ClipWorkflow::Pausing );
        cw->wake();
    }
    else if ( cw->getState() == ClipWorkflow::Initializing )
    {
        qDebug() << "Pausing a Initializing ClipWorkflow";
        cw->getStateLock()->unlock();
        //TODO: since a Initializing clipworkflow will pause itself at the end, shouldn't we do nothing ?
        cw->waitForCompleteInit();
    }
    else
    {
        qDebug() << "Unexpected ClipWorkflow::State when pausing:" << cw->getState();
        cw->getStateLock()->unlock();
    }
    qDebug() << "Waiting for pausing state";
    cw->waitForPausingState();
    qDebug() << "Waiting ok. Pausing now... ";
    cw->pause();
    qDebug() << "Clip paused.";
}

void                TrackWorkflow::pause()
{
    QReadLocker     lock( m_clipsLock );

    QMap<qint64, ClipWorkflow*>::iterator       it = m_clips.begin();
    QMap<qint64, ClipWorkflow*>::iterator       end = m_clips.end();

    //FIXME: it's probably bad to iterate over every clip workflows.
    qDebug() << "Started track pause loop";
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
        if ( cw->getState() != ClipWorkflow::Paused )
        {
            cw->getStateLock()->unlock();
            m_nbClipToPause.fetchAndAddAcquire( 1 );
            connect( cw->getMediaPlayer(), SIGNAL( paused() ), this, SLOT( clipWorkflowPaused() ) );
            pauseClipWorkflow( cw );
        }
        else
        {
            //This should never be used.
            //TODO: remove this in a few revision (wrote on July 16 2009 )
            qDebug() << "Asking to pause in an already paused state";
            cw->getStateLock()->unlock();
        }
    }
    qDebug() << "End of loop";
    m_paused = !m_paused;
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
//            stopClipWorkflow( cw );
//            delete cw;
            computeLength();
            return clip;
        }
        ++it;
    }
    return NULL;
}

void        TrackWorkflow::activateOneFrameOnly()
{
    m_oneFrameOnly = 1;
}

void        TrackWorkflow::clipWorkflowPaused()
{
    m_nbClipToPause.fetchAndAddAcquire( -1 );
    if ( m_nbClipToPause == 0 )
    {
        emit trackPaused();
        qDebug() << "Emitted track paused";
    }
}
