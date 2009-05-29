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

TrackWorkflow::TrackWorkflow() : m_requiredPosition( -1.0f )
{
    m_condMutex = new QMutex;
    m_waitCondition = new QWaitCondition;
    m_mediaPlayer = new LibVLCpp::MediaPlayer();
    m_renderMutex = new QMutex;
    m_requiredPositionLock = new QMutex;
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
    delete m_renderMutex;
}

void    TrackWorkflow::addClip( Clip* clip, qint64 start )
{
    qDebug() << "Inserting clip at frame nb" << start;
    ClipWorkflow* cw = new ClipWorkflow( clip, m_renderMutex, m_condMutex, m_waitCondition );
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
        m_clips.begin().value()->initialize( m_mediaPlayer );
        if ( m_current.key() == 0 )
        {
            m_current = m_clips.begin();
            m_current.value()->startRender();
        }
    }
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

bool                TrackWorkflow::checkNextClip( qint64 currentFrame )
{
    QMap<qint64, ClipWorkflow*>::iterator               next;
    const QMap<qint64, ClipWorkflow*>::const_iterator   end = m_clips.end();

    //Picking next clip :
    if ( m_current == end )
    {
        //Checking if there is a clip in the first place...
        if ( m_clips.count() == 0 )
            return false;
        next = m_clips.begin();
    }
    else
    {
        next = m_clips.begin() + 1;
        if ( next == end )
            return false;
    }

    //If it's about to be used, initialize it
    if ( next.key() == currentFrame + TrackWorkflow::nbFrameBeforePreload  )
    {
        //Don't do anything if the current media player is still in use
        //But for it to be in use, we should have a current media :
        if ( m_current != end && m_current.value()->isRendering() == false )
            qDebug() << "Preloading media";
        next.value()->initialize( m_mediaPlayer );
    }
    //This ClipWorkflow must start at this frame :
    else if ( next.key() == currentFrame )
    {
        qDebug() << "Starting rendering";
        m_current = next;
        m_current.value()->startRender();
    }
    return true;
}

unsigned char*      TrackWorkflow::getOutput( qint64 currentFrame )
{
    unsigned char*  ret = TrackWorkflow::blackOutput;
    bool            clipsRemaining;

    QMutexLocker        lock( m_requiredPositionLock );
    if ( m_requiredPosition >= 0.0f )
    {
        setPosition( m_requiredPosition );
        m_requiredPosition = -1.0f;
    }
//    qDebug() << "Frame nb" << currentFrame;
    clipsRemaining = checkNextClip( currentFrame );
    //This is true only before the first render.
    if ( m_current == m_clips.end() )
    {
        //If the track was empty, then its end is reached
        if ( clipsRemaining == false )
            emit endReached();
        //Else, we return a black screen.
        return TrackWorkflow::blackOutput;
    }
    //We proceed to the render only if the ClipWorkflow is in rendering mode.
    if ( m_current.value()->isRendering() == true )
    {
        m_waitCondition->wakeAll();

        ret = m_current.value()->getOutput();
        return ret;
    }
    else if ( m_current.value()->getState() == ClipWorkflow::EndReached ||
              m_current.value()->getState() == ClipWorkflow::StopScheduled )
    {
        //First, we stop the current ClipWorkflow so that it won't
        //enter the lock/unlock cycle anymore.
        m_current.value()->stop();
        //Then, if there's no remaining clip, end of track is reached.
        if ( clipsRemaining == false )
            emit endReached();
    }
    return ret;
}

void        TrackWorkflow::initializeClipWorkflow( ClipWorkflow* cw )
{
    //>Launching the initialization
    cw->initialize( m_mediaPlayer );
    cw->startRender();
}

void        TrackWorkflow::setPosition( float pos )
{
    qint64                                      frame = (float)m_length * pos;
    QMap<qint64, ClipWorkflow*>::iterator       it = m_clips.begin();
    const QMap<qint64, ClipWorkflow*>::iterator end = m_clips.end();
    QMap<qint64, ClipWorkflow*>::iterator       next = end;

    if ( frame > m_length )
    {
        if ( m_current != end )
        {
            m_current.value()->scheduleStop();
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
                next = next - 1; //Since the iterator must point to the previous video
            }
            else
            {
                next = end;
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
            m_current.value()->scheduleStop();
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
        it.value()->setPosition( (float)( frame - it.key() ) / (float)(it.value()->getClip()->getLength()) );
        //Awaking renderers to avoid them to be stuck inside of the lock...
        m_waitCondition->wakeAll();
    }
    // Else, we found a clip that is not the current one.
    else
    {
        //First, we stop the current workflow.
        if ( m_current != end )
        {
            m_current.value()->scheduleStop();
        }
        //We initialize the new workflow
        initializeClipWorkflow( it.value() );
        //And this is now our current clip
        m_current = it;

        //TODO: we doesn't take the new position in count :/
    }
}

void    TrackWorkflow::requirePositionChanged( float pos )
{
//    QMutexLocker    lock( m_requiredPositionLock );
//    m_requiredPosition = pos;
}
