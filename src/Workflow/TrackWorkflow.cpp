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

#include "TrackWorkflow.h"

unsigned char*  TrackWorkflow::blackOutput = NULL;

TrackWorkflow::TrackWorkflow()
{
    m_condMutex = new QMutex;
    m_waitCondition = new QWaitCondition;
    m_mediaPlayer = new LibVLCpp::MediaPlayer();
    m_currentLock = new QReadWriteLock();
    if ( TrackWorkflow::blackOutput == NULL )
    {
        //TODO: this ain't free !
        TrackWorkflow::blackOutput = new unsigned char[VIDEOHEIGHT * VIDEOWIDTH * 3];
        memset( TrackWorkflow::blackOutput, 0, VIDEOHEIGHT * VIDEOWIDTH * 3 );
    }
}

TrackWorkflow::~TrackWorkflow()
{
    delete m_condMutex;
    delete m_waitCondition;
    delete m_mediaPlayer;
    delete m_currentLock;
}

void    TrackWorkflow::addClip( Clip* clip, qint64 start )
{
    qDebug() << "Inserting clip at frame nb" << start;
    ClipWorkflow* cw = new ClipWorkflow( clip, m_condMutex, m_waitCondition );
    m_clips.insert( start, cw );
    computeLength();
}

void    TrackWorkflow::startRender()
{
    m_current = m_clips.end();

    if ( m_clips.size() <= 0)
        return ;
    //If the first frame is to be render soon, we should play it now.
    if ( m_clips.begin().key() < TrackWorkflow::nbFrameBeforePreload )
    {
//        qDebug() << "Next clip is less than" << nbFrameBeforePreload<< "frame ahead";
        m_clips.begin().value()->initialize( m_mediaPlayer );
//        qDebug() << "Waiting for the first clip to be ready";
        //We wait to be sure the ClipWorkflow will be ready when we really start to render.
        while ( m_clips.begin().value()->isReady() == false )
            usleep( 20 );
        if ( m_current.key() == 0 )
        {
            m_current = m_clips.begin();
//            qDebug() << "Clip workflow is at first frame";
            m_current.value()->startRender();
        }
    }
}

bool                TrackWorkflow::checkNextClip( qint64 currentFrame )
{
    QMap<qint64, ClipWorkflow*>::iterator       next;

    //Picking next clip :
    if ( m_current == m_clips.end() )
    {
        //Checking if there is a clip in the first place...
        if ( m_clips.count() == 0 )
            return false;
        next = m_clips.begin();
    }
    else
    {
        next = m_clips.begin() + 1;
        if ( next == m_clips.end() )
            return false;
    }

    //If it's about to be used, initialize it
    if ( next.key() == currentFrame + TrackWorkflow::nbFrameBeforePreload )
    {
        qDebug() << "Initializing next clipWorkflow";
        next.value()->initialize( m_mediaPlayer );
    }
    else if ( next.key() == currentFrame )
    {
        while ( next.value()->isReady() == false )
            usleep( 20 );
        qDebug() << "Switching current clip workflow";
        //Using it as the current clip from now on.
        m_current = next;
        m_current.value()->startRender();
    }
    return true;
}

void                TrackWorkflow::computeLength()
{
    if ( m_clips.count() == 0 )
        m_length = 0;
    QMap<qint64, ClipWorkflow*>::const_iterator it = m_clips.end() - 1;
    m_length = (it.key() + it.value()->getClip()->getLength() );
}

qint64              TrackWorkflow::getLength() const
{
    return m_length;
}

unsigned char*      TrackWorkflow::getOutput( qint64 currentFrame )
{
    unsigned char*  ret = TrackWorkflow::blackOutput;
    bool            clipsRemaining;

    QReadLocker     lock( m_currentLock );

    qDebug() << "Frame nb" << currentFrame;
    clipsRemaining = checkNextClip( currentFrame );
    if ( m_current == m_clips.end() )
    {
        if ( clipsRemaining == false )
        {
            qDebug() << "End Reached";
            emit endReached();
        }
//        qDebug() << "Stil no clip at this time, going to the next frame";
        return ret;
    }
//    qDebug() << "All waked"
    if ( m_current.value()->isStopped() == false )
    {
        m_waitCondition->wakeAll();

        // We wait until the end of the render. If the clip reach end, this
        // will also return true.
        while ( m_current.value()->renderComplete() == false )
            usleep( 20 );
        if ( m_current.value()->isEndReached() == false )
            ret = m_current.value()->getOutput();
        return ret;
    }
    qDebug() << "End reached : stopping current clipworkflow";
    stopCurrentClipWorkflow();
    if ( clipsRemaining == false )
    {
        qDebug() <<"End reached";
        emit endReached();
    }
    return ret;
}

void        TrackWorkflow::stopCurrentClipWorkflow()
{
    qDebug() << "About to stopCurrentClipWorkflow()";
    //Awaking renderer thread to avoid dead lock
    m_current.value()->scheduleStop();
    m_waitCondition->wakeAll();
    if ( m_current.value()->isStopped() == false )
        m_current.value()->stop();
    qDebug() << "Stopped stopCurrentClipWorkflow();";
}

void        TrackWorkflow::initializeClipWorkflow( ClipWorkflow* cw )
{
    //>Launching the initialization
    qDebug()<< "Initializing clip workflow (initializeClipWorkflow())";
    cw->initialize( m_mediaPlayer );
    //And then wait for it to be ready
    while ( cw->isReady() == false )
        usleep( 20 );
    //Once it is, we actually start the render
    cw->startRender();
    qDebug() << "End of clip workflow reinitialization";
}

void        TrackWorkflow::setPosition( float pos )
{
    qint64                                      frame = (float)m_length * pos;
    QMap<qint64, ClipWorkflow*>::iterator       it = m_clips.begin();
    const QMap<qint64, ClipWorkflow*>::iterator end = m_clips.end();
    QMap<qint64, ClipWorkflow*>::iterator       next = end;

    QWriteLocker        lock( m_currentLock );
    if ( frame > m_length )
    {
//        qDebug() << "setting position after the end of this track";
        if ( m_current != end )
        {
            stopCurrentClipWorkflow();
            m_current = end;
            return ;
        }
    }

    //Locate the new clip workflow
    while ( it != end )
    {
        if ( it.key() <= frame &&
             ( it.key() + it.value()->getClip()->getLength() ) > frame )
        {
//            qDebug() << "Found new current clip workflow";
            break;
        }
        else if ( next == m_clips.end() && it.key() > frame )
        {
            // If this clip doesn't match, but starts AFTER the frame we aim,
            // we can assume that it's the next clip.
            // We can break, and put it to end() in order to simulate the
            // normal end of the loop.
            next = it;
            if ( next != m_clips.begin() )
            {
//                qDebug() << "Next clip isn't the first one";
                next = next - 1; //Since the iterator must point to the previous video
            }
            else
            {
                next = end;
//                qDebug() << "Next clip is the first of the track";
            }
            // in order to checkNextClip() to work.
            it = end;
            break ;
        }
        ++it;
    }

    //No clip was found :
    if ( it == end )
    {
        //We should use the next clip, however, we use the clip just before
        //the next.
        //We also stop the current clip if it was started.
        if ( m_current != end )
        {
            stopCurrentClipWorkflow();
        }
        m_current = next;
    }
    // If the clip found is the current, we just change the position of the
    // media player
    else if ( it == m_current )
    {
        //The clip may have been stoped (if we reached end but came back at it)
        if ( it.value()->isStopped() )
        {
            initializeClipWorkflow( it.value() );
        }
        qDebug() << "Changing the position of the current clip";
        it.value()->setPosition( (float)( frame - it.key() ) / (float)(it.value()->getClip()->getLength()) );
        //Awaking renderers to avoid them to be stuck inside of the lock...
//        qDebug() << "Waking all renderer threads";
        m_waitCondition->wakeAll();
    }
    // Else, we found a clip that is not the current one.
    else
    {
//        qDebug() << "Switching to other Clip";
        //First, we stop the current workflow.
        if ( m_current != end )
        {
            stopCurrentClipWorkflow();
        }
        //We initialize the new workflow
        initializeClipWorkflow( it.value() );
        //And this is now our current clip
        m_current = it;
//        qDebug() << "Switched current clip workflow";
    }
}
