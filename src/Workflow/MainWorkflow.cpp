/*****************************************************************************
 * MainWorkflow.cpp : Will query all of the track workflows to render the final
 *                    image
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

#include "MainWorkflow.h"

unsigned char*  MainWorkflow::blackOutput = NULL;

MainWorkflow::MainWorkflow( int trackCount ) :
        m_trackCount( trackCount ),
        m_renderStarted( false )
{
    if ( MainWorkflow::blackOutput == NULL )
    {
        //TODO: this ain't free !
        MainWorkflow::blackOutput = new unsigned char[VIDEOHEIGHT * VIDEOWIDTH * 3];
        memset( MainWorkflow::blackOutput, 0, VIDEOHEIGHT * VIDEOWIDTH * 3 );
    }

    m_tracks = new Toggleable<TrackWorkflow*>[trackCount];
    for (int i = 0; i < trackCount; ++i)
    {
        m_tracks[i].setPtr( new TrackWorkflow( i ) );
        connect( m_tracks[i], SIGNAL( trackEndReached( unsigned int ) ), this, SLOT( trackEndReached(unsigned int) ) );
    }
}

void    MainWorkflow::addClip( Clip* clip, unsigned int trackId, qint64 start )
{
    Q_ASSERT_X( trackId < m_trackCount, "MainWorkflow::addClip",
                "The specified trackId isn't valid, for it's higher than the number of tracks");

    qDebug() << "MainWorkflow: Adding clip" << clip->getUuid() << "to track" << trackId;
    m_tracks[trackId]->addClip( clip, start );
}

void    MainWorkflow::startRender()
{
    qint64      maxLength = 0;

    m_renderStarted = true;
    m_currentFrame = 0;
    emit frameChanged( 0 );
    for ( unsigned int i = 0; i < m_trackCount; ++i )
    {
        if ( m_tracks[i]->getLength() > maxLength )
            maxLength = m_tracks[i]->getLength();
    }
    m_length = maxLength;
}

unsigned char*    MainWorkflow::getOutput()
{
    unsigned char* ret;

    for ( unsigned int i = 0; i < m_trackCount; ++i )
    {
        if ( m_tracks[i].activated() == false )
            continue ;
        if ( ( ret = m_tracks[i]->getOutput( m_currentFrame ) ) != NULL )
        {
            qDebug() << "Getting a frame";
            break ;
        }
    }
    if ( ret == NULL )
        ret = MainWorkflow::blackOutput;

    ++m_currentFrame;
    emit frameChanged( m_currentFrame );
    emit positionChanged( (float)m_currentFrame / (float)m_length );
    return ret;
}

void        MainWorkflow::setPosition( float pos )
{
    if ( m_renderStarted == false )
        return ;
    qint64  frame = (float)m_length * pos;
    m_currentFrame = frame;
    emit frameChanged( frame );
    //Do not emit a signal for the RenderWidget, since it's the one that triggered that call...
}

qint64      MainWorkflow::getLength() const
{
    return m_length;
}

void        MainWorkflow::trackEndReached( unsigned int trackId )
{
    m_tracks[trackId].deactivate();

    for ( unsigned int i = 0; i < m_trackCount; ++i)
    {
        if ( m_tracks[i].activated() == true )
            return ;
    }
    emit mainWorkflowEndReached();
    m_renderStarted = false;
    m_currentFrame = 0;
    emit frameChanged( 0 );
}

unsigned int    MainWorkflow::getTrackCount() const
{
    return m_trackCount;
}
