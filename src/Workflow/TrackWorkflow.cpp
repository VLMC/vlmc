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
#include "VideoClipWorkflow.h"
#include "ImageClipWorkflow.h"
#include "AudioClipWorkflow.h"
#include "Clip.h"
#include "Media.h"
#include <QReadWriteLock>
#include <QDomDocument>
#include <QDomElement>

TrackWorkflow::TrackWorkflow( unsigned int trackId, MainWorkflow::TrackType type  ) :
        m_trackId( trackId ),
        m_length( 0 ),
        m_forceRepositionning( false ),
        m_paused( false ),
        m_trackType( type ),
        m_lastFrame( 0 ),
        m_videoStackedBuffer( NULL ),
        m_audioStackedBuffer( NULL )
{
    m_forceRepositionningMutex = new QMutex;
    m_renderOneFrameMutex = new QMutex;
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
    delete m_renderOneFrameMutex;
    delete m_forceRepositionningMutex;
}

void    TrackWorkflow::addClip( Clip* clip, qint64 start )
{
    ClipWorkflow* cw;
    if ( m_trackType == MainWorkflow::VideoTrack )
    {
        if ( clip->getParent()->getFileType() == Media::Video )
            cw = new VideoClipWorkflow( clip );
        else
            cw = new ImageClipWorkflow( clip );
    }
    else
        cw = new AudioClipWorkflow( clip );
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

Clip*               TrackWorkflow::getClip( const QUuid& uuid )
{
    QMap<qint64, ClipWorkflow*>::const_iterator     it = m_clips.begin();
    QMap<qint64, ClipWorkflow*>::const_iterator     end = m_clips.end();

    while ( it != end )
    {
        if ( it.value()->getClip()->getUuid() == uuid )
            return it.value()->getClip();
        ++it;
    }
    return NULL;
}

void*       TrackWorkflow::renderClip( ClipWorkflow* cw, qint64 currentFrame,
                                        qint64 start , bool needRepositioning,
                                        bool renderOneFrame )
{
    ClipWorkflow::GetMode       mode = ( m_paused == false || renderOneFrame == true ?
                                         ClipWorkflow::Pop : ClipWorkflow::Get );

    cw->getStateLock()->lockForRead();
//    qDebug() << "TrackWorkflow::renderClip. currentFrame:" << currentFrame << "trackType:" << m_trackType;
    if ( cw->getState() == ClipWorkflow::Rendering ||
         cw->getState() == ClipWorkflow::Paused ||
         cw->getState() == ClipWorkflow::PauseRequired ||
         cw->getState() == ClipWorkflow::UnpauseRequired )
    {
        cw->getStateLock()->unlock();
        if ( needRepositioning == true )
        {
            adjustClipTime( currentFrame, start, cw );
        }
        return cw->getOutput( mode );
    }
    else if ( cw->getState() == ClipWorkflow::Stopped )
    {
        cw->getStateLock()->unlock();
        cw->initialize();
        cw->waitForCompleteInit();
        if ( start != currentFrame || cw->getClip()->getBegin() != 0 ) //Clip was not started as its real begining
            adjustClipTime( currentFrame, start, cw );
        return cw->getOutput( mode );
    }
    else if ( cw->getState() == ClipWorkflow::EndReached )
    {
        cw->getStateLock()->unlock();
        //The stopClipWorkflow() method will take care of that.
    }
    else
    {
        qCritical() << "Unexpected state:" << cw->getState();
        cw->getStateLock()->unlock();
    }
    return NULL;
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
//    qDebug() << "Stopping clip workflow";
    cw->getStateLock()->lockForRead();

    if ( cw->getState() == ClipWorkflow::Stopped )
    {
        cw->getStateLock()->unlock();
        return ;
    }
    if ( cw->getState() == ClipWorkflow::EndReached ||
         cw->getState() == ClipWorkflow::Rendering )
    {
        cw->getStateLock()->unlock();
        cw->stop();
    }
    else
    {
        qCritical() << "Unexpected ClipWorkflow::State when stopping :" << cw->getState();
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
    releasePreviousRender();
    m_lastFrame = 0;
}

void                TrackWorkflow::releasePreviousRender()
{
    if ( m_audioStackedBuffer != NULL )
    {
        m_audioStackedBuffer->release();
        m_audioStackedBuffer = NULL;
    }
    if ( m_videoStackedBuffer != NULL )
    {
        m_videoStackedBuffer->release();
        m_videoStackedBuffer = NULL;
    }
}

void*               TrackWorkflow::getOutput( qint64 currentFrame, qint64 subFrame )
{
    releasePreviousRender();
    QReadLocker     lock( m_clipsLock );

    QMap<qint64, ClipWorkflow*>::iterator       it = m_clips.begin();
    QMap<qint64, ClipWorkflow*>::iterator       end = m_clips.end();
    bool                                        needRepositioning;
    void*                                       ret = NULL;
    bool                                        renderOneFrame = false;

    if ( checkEnd( currentFrame ) == true )
    {
        emit trackEndReached( m_trackId );
        //We continue, as there can be ClipWorkflow that requires to be stopped.
    }
    {
        QMutexLocker      lock2( m_renderOneFrameMutex );
        if ( m_renderOneFrame == true )
        {
            m_renderOneFrame = false;
            renderOneFrame = true;
        }
    }
    {
        QMutexLocker    lock2( m_forceRepositionningMutex );
        if ( m_forceRepositionning == true )
        {
            needRepositioning = true;
            m_forceRepositionning = false;
        }
        // This is a bit hackish : when we want to pop a frame in renderOneFrame mode,
        // we also set the position to avoid the stream to be missynchronized.
        // this frame setting will most likely toggle the next condition as true
        // If this condition is true, the clipworkflow will flush all its buffer
        // as we need to resynchronize after a setTime, so this condition has to remain
        // false. Easy ain't it ?
        else if ( m_paused == true && subFrame != m_lastFrame && renderOneFrame == false)
        {
            needRepositioning = true;
        }
        else
            needRepositioning = ( abs( subFrame - m_lastFrame ) > 1 ) ? true : false;
    }

    while ( it != end )
    {
        qint64          start = it.key();
        ClipWorkflow*   cw = it.value();
        //Is the clip supposed to render now ?
//        qDebug() << "Start:" << start << "Current Frame:" << currentFrame;
        if ( start <= currentFrame && currentFrame <= start + cw->getClip()->getLength() )
        {
            if ( ret != NULL )
                qCritical() << "There's more than one clip to render here. Undefined behaviour !";
            ret = renderClip( cw, currentFrame, start, needRepositioning, renderOneFrame );
            if ( m_trackType == MainWorkflow::VideoTrack )
                m_videoStackedBuffer = reinterpret_cast<StackedBuffer<LightVideoFrame*>*>( ret );
            else
                m_audioStackedBuffer = reinterpret_cast<StackedBuffer<AudioClipWorkflow::AudioSample*>*>( ret );
        }
        //Is it about to be rendered ?
        else if ( start > currentFrame &&
                start - currentFrame < TrackWorkflow::nbFrameBeforePreload )
            preloadClip( cw );
        //Is it supposed to be stopped ?
        else
            stopClipWorkflow( cw );
        ++it;
    }
    m_lastFrame = subFrame;

    return ret;
}

void                TrackWorkflow::pause()
{
    m_paused = true;
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
            stopClipWorkflow( cw );
            computeLength();
            cw->disconnect();
            delete cw;
            if ( m_length == 0 )
                emit trackEndReached( m_trackId );
            return clip;
        }
        ++it;
    }
    return NULL;
}

ClipWorkflow*       TrackWorkflow::removeClipWorkflow( const QUuid& id )
{
    QWriteLocker    lock( m_clipsLock );

    QMap<qint64, ClipWorkflow*>::iterator       it = m_clips.begin();
    QMap<qint64, ClipWorkflow*>::iterator       end = m_clips.end();

    while ( it != end )
    {
        if ( it.value()->getClip()->getUuid() == id )
        {
            ClipWorkflow*   cw = it.value();
            cw->disconnect();
            m_clips.erase( it );
            computeLength();
            return cw;

        }
        ++it;
    }
    return NULL;
}

void    TrackWorkflow::unpause()
{
    m_paused = false;
}

void    TrackWorkflow::save( QDomDocument& doc, QDomElement& trackNode ) const
{
    QReadLocker     lock( m_clipsLock );

    QMap<qint64, ClipWorkflow*>::const_iterator     it = m_clips.begin();
    QMap<qint64, ClipWorkflow*>::const_iterator     end = m_clips.end();

    for ( ; it != end ; ++it )
    {
        QDomElement     clipNode = doc.createElement( "clip" );

        {
            QDomElement     parent = doc.createElement( "parent" );

            QDomCharacterData   text = doc.createTextNode( it.value()->getClip()->getParent()->getUuid().toString() );
            parent.appendChild( text );
            clipNode.appendChild( parent );
        }
        {
            QDomElement     startFrame = doc.createElement( "startFrame" );

            QDomCharacterData   text = doc.createTextNode( QString::number( it.key() ) );
            startFrame.appendChild( text );
            clipNode.appendChild( startFrame );
        }
        {
            QDomElement     begin = doc.createElement( "begin" );

            QDomCharacterData   text = doc.createTextNode( QString::number( it.value()->getClip()->getBegin() ) );
            begin.appendChild( text );
            clipNode.appendChild( begin );
        }
        {
            QDomElement     end = doc.createElement( "end" );

            QDomCharacterData   text = doc.createTextNode( QString::number( it.value()->getClip()->getEnd() ) );
            end.appendChild( text );
            clipNode.appendChild( end );
        }
        {
            QDomElement     trackType = doc.createElement( "trackType" );

            QDomCharacterData   text = doc.createTextNode( QString::number( m_trackType ) );
            trackType.appendChild( text );
            clipNode.appendChild( trackType );
        }
        trackNode.appendChild( clipNode );
    }
}

void    TrackWorkflow::clear()
{
    QWriteLocker    lock( m_clipsLock );
    QMap<qint64, ClipWorkflow*>::iterator       it = m_clips.begin();
    QMap<qint64, ClipWorkflow*>::iterator       end = m_clips.end();

    for ( ; it != end; ++it )
    {
        //The clip contained in the trackworkflow will be delete by the undo stack.
        ClipWorkflow*   cw = it.value();
        delete cw->getClip();
        delete cw;
    }
    m_clips.clear();
    m_length = 0;
}

void    TrackWorkflow::adjustClipTime( qint64 currentFrame, qint64 start, ClipWorkflow* cw )
{
    qint64  nbMs = ( currentFrame - start ) / cw->getClip()->getParent()->getFps() * 1000;
    qint64  beginInMs = cw->getClip()->getBegin() / cw->getClip()->getParent()->getFps() * 1000;
    qint64  startFrame = beginInMs + nbMs;
    cw->setTime( startFrame );
}

void    TrackWorkflow::forceRepositionning()
{
    QMutexLocker    lock( m_forceRepositionningMutex );
    m_forceRepositionning = true;
}

void
TrackWorkflow::renderOneFrame()
{
    QMutexLocker    lock( m_renderOneFrameMutex );
    m_renderOneFrame = true;
}
