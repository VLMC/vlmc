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

TrackWorkflow::TrackWorkflow() : m_isRendering( false )
{
    m_condMutex = new QMutex;
    m_waitCondition = new QWaitCondition;
    m_mediaPlayer = new LibVLCpp::MediaPlayer();
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
        while ( m_clips.begin().value()->isReady() == false )
            usleep( 150 );
        if ( m_current.key() == 0 )
        {
            m_current = m_clips.begin();
//            qDebug() << "Clip workflow is at first frame";
            m_current.value()->startRender();
            m_isRendering = true;
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

unsigned char*      TrackWorkflow::getOutput( qint64 currentFrame )
{
    unsigned char*  ret = TrackWorkflow::blackOutput;
    bool            clipsRemaining;

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
