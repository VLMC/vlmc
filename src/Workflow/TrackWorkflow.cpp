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
        TrackWorkflow::blackOutput = new unsigned char[VIDEOHEIGHT * VIDEOWIDTH * 3];
        memset( TrackWorkflow::blackOutput, 0, VIDEOHEIGHT * VIDEOWIDTH * 3 );
    }
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
    //If the first frame is to be render soon, we should play it now.
    if ( m_clips.begin().key() < TrackWorkflow::nbFrameBeforePreload )
    {
//        qDebug() << "Next clip is less than" << nbFrameBeforePreload<< "frame ahead";
        m_clips.begin().value()->initialize( m_mediaPlayer );
//        qDebug() << "Waiting for the first clip to be ready";
        //We wait to be sure the ClipWorkflow will be ready when we really start to render.
        while ( m_clips.begin().value()->isReady() == false )
            usleep( 150 );
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
//        qDebug() << "Initializing next clipWorkflow";
        next.value()->initialize( m_mediaPlayer );
    }
    else if ( next.key() == currentFrame )
    {
        //It should have been initialized now, however, this ain't very safe :/
        Q_ASSERT( next.value()->isReady() );
//        qDebug() << "Switching current clip workflow";
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
    qDebug() << "Last clip Uuid : " << it.value()->getClip()->getUuid();
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

//    qDebug() << "Frame nb" << m_currentFrame;
    clipsRemaining = checkNextClip( currentFrame );
    if ( m_current == m_clips.end() )
    {
//        qDebug() << "Stil no clip at this time, going to the next frame";
        return ret;
    }
    m_waitCondition->wakeAll();
//    qDebug() << "All waked";
    while ( m_current.value()->renderComplete() == false )
        usleep( 20 );
    if ( m_current.value()->isEndReached() == false )
        ret = m_current.value()->getOutput();
    else
    {
        if ( clipsRemaining == false )
        {
//            qDebug() << "End of track";
            return NULL;
        }
    }
    return ret;
}

void        TrackWorkflow::setPosition( float pos )
{
    qint64                                  frame = (float)m_length * pos;
    QMap<qint64, ClipWorkflow*>::iterator   it = m_clips.begin();
    const QMap<qint64, ClipWorkflow*>::iterator   end = m_clips.end();
    QMap<qint64, ClipWorkflow*>::iterator   next = m_clips.end();

    QWriteLocker        lock( m_currentLock );
    qDebug() << m_length;
    if ( frame > m_length )
    {
        qDebug() << "setting position after the end of this track";
        if ( m_current != end )
        {
            m_current.value()->stop();
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
            qDebug() << "Found new current clip workflow";
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
                qDebug() << "Next clip isn't the first one";
                next = next - 1; //Since the iterator must point to the previous video
            }
            else
            {
                next = end;
                qDebug() << "Next clip is the first of the track";
            }
            // in order to checkNextClip() to work.
            it = end;
            break ;
        }
        ++it;
    }

    if ( it == m_clips.end() )
    {
        qDebug() << "No clip matched. Utilisation du Clip precedent le clip suivant la frame selectionnee";
        if ( m_current != end )
            m_current.value()->stop();
        m_current = next;
    }
    else if ( it == m_current )
    {
        qDebug() << "Changing the position of the current clip";
        //We're changing the position of the current clip
//        qDebug() << "frame =" << frame << " key = "<< it.key() <<
        it.value()->setPosition( (float)( frame - it.key() ) / (float)(it.value()->getClip()->getLength()) );
        //Awaking renderers to avoid them to be stuck inside of the lock...
        qDebug() << "Waking all renderer threads";
        m_waitCondition->wakeAll();
    }
    else
    {
        qDebug() << "Switching to other Clip";
        if ( m_current != end )
        {
            m_current.value()->stop();
//            m_waitCondition->wakeAll();
        }

        it.value()->initialize( m_mediaPlayer );
        while ( it.value()->isReady() == false )
            usleep( 150 );
        it.value()->startRender();
        m_current = it;
        qDebug() << "Switched current clip workflow";
    }

    //Don't forget to wake the renderer so they can process the events
}
