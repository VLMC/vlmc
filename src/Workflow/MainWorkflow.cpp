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
        m_currentFrame( 0 ),
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
    for ( int i = 0; i < trackCount; ++i )
    {
        m_tracks[i].setPtr( new TrackWorkflow( i, TrackWorkflow::Video ) );
        connect( m_tracks[i], SIGNAL( trackEndReached( unsigned int ) ), this, SLOT( trackEndReached(unsigned int) ) );
        connect( m_tracks[i], SIGNAL( trackPaused() ), this, SLOT( trackPaused() ) );
        connect( m_tracks[i], SIGNAL( trackUnpaused() ), this, SLOT( trackUnpaused() ) );
        connect( m_tracks[i], SIGNAL( renderCompleted( unsigned int ) ), this,  SLOT( tracksRenderCompleted( unsigned int ) ), Qt::QueuedConnection );
    }
    m_renderStartedLock = new QReadWriteLock;
    m_renderMutex = new QMutex;
    m_highestTrackNumberMutex = new QMutex;
    m_synchroneRenderWaitCondition = new QWaitCondition;
    m_synchroneRenderWaitConditionMutex = new QMutex;
    m_nbTracksToRenderMutex = new QMutex;
}

MainWorkflow::~MainWorkflow()
{
    //FIXME: this is probably useless, since already done by the renderer
    stop();

    delete m_nbTracksToRenderMutex;
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

void        MainWorkflow::addClip( Clip* clip, unsigned int trackId, qint64 start )
{
    Q_ASSERT_X( trackId < m_trackCount, "MainWorkflow::addClip",
                "The specified trackId isn't valid, for it's higher than the number of tracks");

    m_tracks[trackId]->addClip( clip, start );
    //if the track is deactivated, we need to reactivate it.
    if ( m_tracks[trackId].deactivated() == true )
        activateTrack( trackId );

    //Now check if this clip addition has changed something about the workflow's length
    if ( m_tracks[trackId]->getLength() > m_length )
        m_length = m_tracks[trackId]->getLength();

    //Inform the GUI
    emit clipAdded( clip, trackId, start );
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
    m_paused = false;
    for ( unsigned int i = 0; i < m_trackCount; ++i )
        activateTrack( i );
    computeLength();
}

void                MainWorkflow::getOutput()
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
        QMutexLocker    lockNbTracks( m_nbTracksToRenderMutex );
        for ( unsigned int i = 0; i < m_trackCount; ++i )
        {
            if ( m_tracks[i].activated() == false )
                continue ;

            ++m_nbTracksToRender;
            m_tracks[i]->getOutput( m_currentFrame );
        }
        if ( m_paused == false )
            nextFrame();
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

void        MainWorkflow::unpause()
{
    QMutexLocker    lock( m_renderMutex );

    m_nbTracksToUnpause = 0;
    for ( unsigned int i = 0; i < m_trackCount; ++i )
    {
        if ( m_tracks[i].activated() == true )
        {
            m_nbTracksToUnpause.fetchAndAddAcquire( 1 );
            m_tracks[i]->unpause();
        }
    }
}

void        MainWorkflow::nextFrame()
{
    ++m_currentFrame;
    emit frameChanged( m_currentFrame );
    emit positionChanged( (float)m_currentFrame / (float)m_length );
}

void        MainWorkflow::previousFrame()
{
    --m_currentFrame;
    emit frameChanged( m_currentFrame );
    emit positionChanged( (float)m_currentFrame / (float)m_length );
}

void        MainWorkflow::setPosition( float pos )
{
    if ( m_renderStarted == true )
    {
        //Since any track can be reactivated, we reactivate all of them, and let them
        //unable themself if required.
        for ( unsigned int i = 0; i < m_trackCount; ++i)
            activateTrack( i );
    }
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
    emit positionChanged( 0 );
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

void           MainWorkflow::moveClip( const QUuid& clipUuid, unsigned int oldTrack,
                                       unsigned int newTrack, qint64 startingFrame, bool undoRedoCommand /*= false*/ )
{
    Q_ASSERT( newTrack < m_trackCount && oldTrack < m_trackCount );

    if ( oldTrack == newTrack )
    {
        //And now, just move the clip.
        m_tracks[newTrack]->moveClip( clipUuid, startingFrame );
        activateTrack( newTrack );
    }
    else
    {
        bool    needRepo;

        if ( m_tracks[oldTrack]->getClipPosition( clipUuid ) != startingFrame )
            needRepo = true;
        ClipWorkflow* cw = m_tracks[oldTrack]->removeClipWorkflow( clipUuid );
        m_tracks[newTrack]->addClip( cw, startingFrame );
        if ( needRepo == true )
            m_tracks[newTrack]->forceRepositionning();
        activateTrack( oldTrack );
        activateTrack( newTrack );
    }
    computeLength();
    if ( undoRedoCommand == true )
    {
        emit clipMoved( clipUuid, newTrack, startingFrame );
    }
}

Clip*       MainWorkflow::removeClip( const QUuid& uuid, unsigned int trackId )
{
    Q_ASSERT( trackId < m_trackCount );

    Clip* clip = m_tracks[trackId]->removeClip( uuid );
    computeLength();
    activateTrack( trackId );
    emit clipRemoved( uuid, trackId );
    return clip;
}

void        MainWorkflow::trackPaused()
{
    m_nbTracksToPause.fetchAndAddAcquire( -1 );
    if ( m_nbTracksToPause <= 0 )
    {
        m_paused = true;
        emit mainWorkflowPaused();
    }
}

void        MainWorkflow::trackUnpaused()
{
    m_nbTracksToUnpause.fetchAndAddAcquire( -1 );
    if ( m_nbTracksToUnpause <= 0 )
    {
        m_paused = false;
        emit mainWorkflowUnpaused();
    }
}

void        MainWorkflow::tracksRenderCompleted( unsigned int trackId )
{
//    qDebug() << "tracksRenderCompleted";
    QMutexLocker    lockNbTracks( m_nbTracksToRenderMutex );
    --m_nbTracksToRender;

    {
        QMutexLocker    lock( m_highestTrackNumberMutex );

        unsigned char* buff = m_tracks[trackId]->getSynchroneOutput();
        if ( m_highestTrackNumber <= trackId && buff != NULL )
        {
            m_highestTrackNumber = trackId;
            m_synchroneRenderingBuffer = buff;;
        }
    }
    //We check for minus or equal, since we can have 0 frame to compute,
    //therefore, m_nbTracksToRender will be equal to -1
    if ( m_nbTracksToRender <= 0 )
    {
//        qDebug() << "main workflow render completed";
        //Just a synchronisation barriere
        {
            QMutexLocker    lock( m_synchroneRenderWaitConditionMutex );
        }
        m_synchroneRenderWaitCondition->wakeAll();
    }
}

unsigned char*  MainWorkflow::getSynchroneOutput()
{
    m_synchroneRenderWaitConditionMutex->lock();
    getOutput();
//    qDebug() << "Waiting for sync output";
    m_synchroneRenderWaitCondition->wait( m_synchroneRenderWaitConditionMutex );
//    qDebug() << "Got it";
    m_synchroneRenderWaitConditionMutex->unlock();
    if ( m_synchroneRenderingBuffer == NULL )
        return MainWorkflow::blackOutput;
    return m_synchroneRenderingBuffer;
}

void        MainWorkflow::cancelSynchronisation()
{
    {
        QMutexLocker    lock( m_synchroneRenderWaitConditionMutex );
    }
    m_synchroneRenderWaitCondition->wakeAll();
}

void        MainWorkflow::muteTrack( unsigned int trackId )
{
    m_tracks[trackId].setHardDeactivation( true );
}

void        MainWorkflow::unmuteTrack( unsigned int trackId )
{
    m_tracks[trackId].setHardDeactivation( false );
}

void        MainWorkflow::setCurrentFrame( qint64 currentFrame )
{
    m_currentFrame = currentFrame;
    emit positionChanged( (float)m_currentFrame / (float)m_length );
}

void        MainWorkflow::activateTrack( unsigned int trackId )
{
    if ( m_tracks[trackId]->getLength() > 0 )
        m_tracks[trackId].activate();
    else
        m_tracks[trackId].deactivate();
}

Clip*       MainWorkflow::getClip( const QUuid& uuid, unsigned int trackId )
{
    Q_ASSERT( trackId < m_trackCount );

    return m_tracks[trackId]->getClip( uuid );
}

void        MainWorkflow::loadProject( const QDomElement& project )
{
    if ( project.isNull() == true || project.tagName() != "timeline" )
    {
        qWarning() << "Invalid timeline node (" << project.tagName() << ')';
        return ;
    }

    clear();

    QDomElement elem = project.firstChild().toElement();

    while ( elem.isNull() == false )
    {
        bool    ok;

        Q_ASSERT( elem.tagName() == "track" );
        unsigned int trackId = elem.attribute( "id" ).toUInt( &ok );
        if ( ok == false )
        {
            qWarning() << "Invalid track number in project file";
            return ;
        }
        QDomElement clip = elem.firstChild().toElement();
        while ( clip.isNull() == false )
        {
            //Iterate over clip fields:
            QDomElement clipProperty = clip.firstChild().toElement();
            QUuid       parent;
            qint64      begin;
            qint64      end;
            qint64      startPos;

            while ( clipProperty.isNull() == false )
            {
                QString tagName = clipProperty.tagName();
                bool    ok;

                if ( tagName == "parent" )
                    parent = QUuid( clipProperty.text() );
                else if ( tagName == "begin" )
                {
                    begin = clipProperty.text().toLongLong( &ok );
                    if ( ok == false )
                    {
                        qWarning() << "Invalid clip begin";
                        return ;
                    }
                }
                else if ( tagName == "end" )
                {
                    end = clipProperty.text().toLongLong( &ok );
                    if ( ok == false )
                    {
                        qWarning() << "Invalid clip end";
                        return ;
                    }
                }
                else if ( tagName == "startFrame" )
                {
                    startPos = clipProperty.text().toLongLong( &ok );
                    if ( ok == false )
                    {
                        qWarning() << "Invalid clip starting frame";
                        return ;
                    }
                }
                else
                    qDebug() << "Unknown field" << clipProperty.tagName();

                clipProperty = clipProperty.nextSibling().toElement();
            }

            Clip*       c = new Clip( parent, begin, end );
            addClip( c, trackId, startPos );

            clip = clip.nextSibling().toElement();
        }
        elem = elem.nextSibling().toElement();
    }
}

void        MainWorkflow::saveProject( QDomDocument& doc, QDomElement& rootNode )
{
    QDomElement project = doc.createElement( "timeline" );
    for ( unsigned int i = 0; i < m_trackCount; ++i )
    {
        if ( m_tracks[i]->getLength() > 0 )
        {
            QDomElement     trackNode = doc.createElement( "track" );

            trackNode.setAttribute( "id", i );

            m_tracks[i]->save( doc, trackNode );
            project.appendChild( trackNode );
        }
    }
    rootNode.appendChild( project );
}

void        MainWorkflow::clear()
{
    for ( unsigned int i = 0; i < m_trackCount; ++i )
    {
        m_tracks[i]->clear();
    }
    m_length = 0;
    emit cleared();
}

void        MainWorkflow::setFullSpeedRender( bool value )
{
    for ( unsigned int i = 0; i < m_trackCount; ++i )
        m_tracks[i]->setFullSpeedRender( value );
}
