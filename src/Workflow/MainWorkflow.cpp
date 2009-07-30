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
MainWorkflow*   MainWorkflow::m_instance = NULL;

MainWorkflow::MainWorkflow( int trackCount ) :
        m_length( 0 ),
        m_trackCount( trackCount ),
        m_renderStarted( false )
{
    Q_ASSERT_X( MainWorkflow::m_instance == NULL,
                "MainWorkflow constructor", "Can't have more than one MainWorkflow instance" );
    m_instance = this;

    MainWorkflow::blackOutput = new unsigned char[VIDEOHEIGHT * VIDEOWIDTH * 3];
    memset( MainWorkflow::blackOutput, 0, VIDEOHEIGHT * VIDEOWIDTH * 3 );

    m_tracks = new Toggleable<TrackWorkflow*>[trackCount];
    for (int i = 0; i < trackCount; ++i)
    {
        m_tracks[i].setPtr( new TrackWorkflow( i ) );
        connect( m_tracks[i], SIGNAL( trackEndReached( unsigned int ) ), this, SLOT( trackEndReached(unsigned int) ) );
        connect( m_tracks[i], SIGNAL( trackPaused() ), this, SLOT( trackPaused() ) );
        connect( m_tracks[i], SIGNAL( renderCompleted( unsigned int ) ), this,  SLOT( tracksRenderCompleted( unsigned int ) ), Qt::QueuedConnection );
    }
    m_renderStartedLock = new QReadWriteLock;
    m_renderMutex = new QMutex;
    m_highestTrackNumberMutex = new QMutex;
    m_synchroneRenderWaitCondition = new QWaitCondition;
    m_synchroneRenderWaitConditionMutex = new QMutex;
}

MainWorkflow::~MainWorkflow()
{
    stop();

    delete m_synchroneRenderWaitConditionMutex;
    delete m_synchroneRenderWaitCondition;
    delete m_highestTrackNumberMutex;
    delete m_renderMutex;
    delete m_renderStartedLock;
    for (unsigned int i = 0; i < m_trackCount; ++i)
        delete m_tracks[i];
    delete[] m_tracks;
    delete[] blackOutput;
}

void    MainWorkflow::addClip( Clip* clip, unsigned int trackId, qint64 start )
{
    Q_ASSERT_X( trackId < m_trackCount, "MainWorkflow::addClip",
                "The specified trackId isn't valid, for it's higher than the number of tracks");

    //if the track is deactivated, we need to reactivate it :
    if ( m_tracks[trackId].deactivated() == true )
        m_tracks[trackId].activate();
    m_tracks[trackId]->addClip( clip, start );
    if ( m_tracks[trackId]->getLength() > m_length )
        m_length = m_tracks[trackId]->getLength();
}

void            MainWorkflow::computeLength()
{
    qint64      maxLength = 0;

    for ( unsigned int i = 0; i < m_trackCount; ++i )
    {
        if ( m_tracks[i]->getLength() > maxLength )
            maxLength = m_tracks[i]->getLength();
    }
    m_length = maxLength;
}

void    MainWorkflow::startRender()
{
    m_renderStarted = true;
    m_currentFrame = 0;
    emit frameChanged( 0 );
    for ( unsigned int i = 0; i < m_trackCount; ++i )
        m_tracks[i].activate();
    computeLength();
}

unsigned char*    MainWorkflow::getOutput()
{
    QReadLocker     lock( m_renderStartedLock );
    QMutexLocker    lock2( m_renderMutex );

    {
        QMutexLocker    lockHighestTrackNumber( m_highestTrackNumberMutex );
        m_highestTrackNumber = 0;
    }
    m_nbTracksToRender = 0;
    m_synchroneRenderingBuffer = NULL;
    if ( m_renderStarted == true )
    {
        unsigned char* ret;

        for ( unsigned int i = 0; i < m_trackCount; ++i )
        {
            if ( m_tracks[i].activated() == false )
                continue ;

            if ( ( ret = m_tracks[i]->getOutput( m_currentFrame ) ) != NULL )
            {
                m_nbTracksToRender.fetchAndAddAcquire( 1 );
                break ;
            }
        }
        if ( ret == NULL )
            ret = MainWorkflow::blackOutput;
        nextFrame();
	qDebug() << "Getoutput complete";
        return ret;
    }
    else
    {
        qDebug() << "Getoutput empty";
        return MainWorkflow::blackOutput;
    }
}

void        MainWorkflow::pause()
{
    QMutexLocker    lock( m_renderMutex );

    m_nbTracksToPause = 0;
    for ( unsigned int i = 0; i < m_trackCount; ++i )
    {
        if ( m_tracks[i].activated() == true )
        {
            m_nbTracksToPause.fetchAndAddAcquire( 1 );
            m_tracks[i]->pause();
        }
    }
}

void        MainWorkflow::nextFrame()
{
    ++m_currentFrame;
    //FIXME: This is probably a bit much...
    emit frameChanged( m_currentFrame );
    emit positionChanged( (float)m_currentFrame / (float)m_length );
}

void        MainWorkflow::previousFrame()
{
    --m_currentFrame;
    //FIXME: This is probably a bit much...
    emit frameChanged( m_currentFrame );
    emit positionChanged( (float)m_currentFrame / (float)m_length );
}

void        MainWorkflow::setPosition( float pos )
{
    //Since any track can be reactivated, we reactivate all of them, and let them
    //unable themself if required.
    for ( unsigned int i = 0; i < m_trackCount; ++i)
        m_tracks[i].activate();

    if ( m_renderStarted == false )
        return ;
    qint64  frame = static_cast<qint64>( (float)m_length * pos );
    m_currentFrame = frame;
    emit frameChanged( frame );
    //Do not emit a signal for the RenderWidget, since it's the one that triggered that call...
}

qint64      MainWorkflow::getLength() const
{
    return m_length;
}

qint64      MainWorkflow::getClipPosition( const QUuid& uuid, unsigned int trackId ) const
{
    Q_ASSERT( trackId < m_trackCount );

    return m_tracks[trackId]->getClipPosition( uuid );
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

void            MainWorkflow::stop()
{
    QWriteLocker    lock( m_renderStartedLock );

    m_renderStarted = false;
    for (unsigned int i = 0; i < m_trackCount; ++i)
    {
        if ( m_tracks[i].activated() == true )
            m_tracks[i]->stop();
    }
    m_currentFrame = 0;
    emit frameChanged( 0 );
}

MainWorkflow*   MainWorkflow::getInstance()
{
    Q_ASSERT( m_instance != NULL );
    return m_instance;
}

void            MainWorkflow::deleteInstance()
{
    if ( m_instance != NULL )
    {
        delete m_instance;
        m_instance = NULL;
    }
}

void           MainWorkflow::moveClip( const QUuid& clipUuid, unsigned int oldTrack, unsigned int newTrack, qint64 startingFrame )
{
    Q_ASSERT( newTrack < m_trackCount && oldTrack < m_trackCount );

    if ( oldTrack == newTrack )
    {
        //And now, just move the clip.
        m_tracks[newTrack]->moveClip( clipUuid, startingFrame );
        m_tracks[newTrack].activate();
    }
    else
    {
        Clip* clip = m_tracks[oldTrack]->removeClip( clipUuid );
        m_tracks[newTrack]->addClip( clip, startingFrame );
        m_tracks[oldTrack].activate();
        m_tracks[newTrack].activate();
    }
    computeLength();
}

Clip*       MainWorkflow::removeClip( const QUuid& uuid, unsigned int trackId )
{
    Q_ASSERT( trackId < m_trackCount );

    Clip* clip = m_tracks[trackId]->removeClip( uuid );
    emit clipRemoved( uuid, trackId );
    return clip;
}

void        MainWorkflow::activateOneFrameOnly()
{
     for (unsigned int i = 0; i < m_trackCount; ++i)
    {
        //FIXME: After debugging period, this should'nt be necessary --
        if ( m_tracks[i].activated() == true )
            m_tracks[i]->activateOneFrameOnly();
    }
}

void        MainWorkflow::trackPaused()
{
    m_nbTracksToPause.fetchAndAddAcquire( -1 );
    if ( m_nbTracksToPause == 0 )
    {
        emit mainWorkflowPaused();
    }
}

void        MainWorkflow::tracksRenderCompleted( unsigned int trackId )
{
    m_nbTracksToRender.fetchAndAddAcquire( -1 );

    {
        QMutexLocker    lock( m_highestTrackNumberMutex );
        if ( m_highestTrackNumber <= trackId )
        {
            m_highestTrackNumber = trackId;
            m_synchroneRenderingBuffer = m_tracks[trackId]->getSynchroneOutput();
        }
    }
    //We check for minus or equal, since we can have 0 frame to compute,
    //therefore, m_nbTracksToRender will be equal to -1
    if ( m_nbTracksToRender <= 0 )
    {
        qDebug() << "MainWorkflow render completed";
        //Just a synchronisation barriere
        {
            QMutexLocker    lock( m_synchroneRenderWaitConditionMutex );
        }
        m_synchroneRenderWaitCondition->wakeAll();
    }
    else
        qDebug() << m_nbTracksToRender << "tracks left to render";
}

unsigned char*  MainWorkflow::getSynchroneOutput()
{
    m_synchroneRenderWaitConditionMutex->lock();
    getOutput();
    qDebug() << "Waiting for synchrone output";
    m_synchroneRenderWaitCondition->wait( m_synchroneRenderWaitConditionMutex );
    qDebug() << "Got it";
    m_synchroneRenderWaitConditionMutex->unlock();
    qDebug() << (void*)m_synchroneRenderingBuffer;
    if ( m_synchroneRenderingBuffer == NULL )
        return MainWorkflow::blackOutput;
    return m_synchroneRenderingBuffer;
}
