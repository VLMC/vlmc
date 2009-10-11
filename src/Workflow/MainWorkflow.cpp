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

//JUST FOR THE DEFINES !
//TODO:
//FIXME: remove this !
#include "ClipWorkflow.h"

unsigned char*  MainWorkflow::blackOutput = NULL;

MainWorkflow::MainWorkflow( int trackCount ) :
        m_currentFrame( 0 ),
        m_length( 0 ),
        m_renderStarted( false )
{
    MainWorkflow::blackOutput = new unsigned char[VIDEOHEIGHT * VIDEOWIDTH * 3];
    memset( MainWorkflow::blackOutput, 0, VIDEOHEIGHT * VIDEOWIDTH * 3 );

    m_renderStartedLock = new QReadWriteLock;
    m_renderMutex = new QMutex;
    m_synchroneRenderWaitCondition = new QWaitCondition;
    m_synchroneRenderWaitConditionMutex = new QMutex;
    m_tracks = new TrackHandler*[2];
    for ( unsigned int i = 0; i < TrackWorkflow::NbType; ++i )
    {
        TrackWorkflow::TrackType trackType = (i == 0 ? TrackWorkflow::Video : TrackWorkflow::Audio );
        m_tracks[i] = new TrackHandler( trackCount, trackType );
        connect( m_tracks[i], SIGNAL( tracksPaused() ), this, SLOT( tracksPaused() ) );
    }
    m_outputBuffers = new OutputBuffers;
}

MainWorkflow::~MainWorkflow()
{
    //FIXME: this is probably useless, since already done by the renderer
    stop();

    delete m_synchroneRenderWaitConditionMutex;
    delete m_synchroneRenderWaitCondition;
    delete m_renderMutex;
    delete m_renderStartedLock;
    delete[] blackOutput;
    delete m_tracks[0];
    delete m_tracks[1];
    delete[] m_tracks;
}

void            MainWorkflow::addClip( Clip* clip, unsigned int trackId,
                                        qint64 start, TrackWorkflow::TrackType trackType )
{
    m_tracks[trackType]->addClip( clip, trackId, start );

    //Inform the GUI
    emit clipAdded( clip, trackId, start, trackType );
}

void            MainWorkflow::computeLength()
{
    qint64      maxLength = 0;

    for ( unsigned int i = 0; i < TrackWorkflow::NbType; ++i )
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
    for ( unsigned int i = 0; i < TrackWorkflow::NbType; ++i )
        m_tracks[i]->startRender();
    computeLength();
}

void                MainWorkflow::getOutput()
{
    QReadLocker     lock( m_renderStartedLock );
    QMutexLocker    lock2( m_renderMutex );

    if ( m_renderStarted == true )
    {
        for ( unsigned int i = 0; i < TrackWorkflow::NbType; ++i )
            m_tracks[i]->getOutput( m_currentFrame );
        if ( m_paused == false )
            nextFrame();
    }
}

void        MainWorkflow::pause()
{
    QMutexLocker    lock( m_renderMutex );

    for ( unsigned int i = 0; i < TrackWorkflow::NbType; ++i )
        m_tracks[i]->pause();
}

void        MainWorkflow::unpause()
{
    QMutexLocker    lock( m_renderMutex );

    for ( unsigned int i = 0; i < TrackWorkflow::NbType; ++i )
        m_tracks[i]->unpause();
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
        for ( unsigned int i = 0; i < TrackWorkflow::NbType; ++i)
            m_tracks[i]->activateAll();
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

qint64      MainWorkflow::getClipPosition( const QUuid& uuid, unsigned int trackId, TrackWorkflow::TrackType trackType ) const
{
    return m_tracks[trackType]->getClipPosition( uuid, trackId );
}

void            MainWorkflow::stop()
{
    QWriteLocker    lock( m_renderStartedLock );

    m_renderStarted = false;
    for (unsigned int i = 0; i < TrackWorkflow::NbType; ++i)
        m_tracks[i]->stop();
    m_currentFrame = 0;
    emit frameChanged( 0 );
    emit positionChanged( 0 );
}

void           MainWorkflow::moveClip( const QUuid& clipUuid, unsigned int oldTrack,
                                       unsigned int newTrack, qint64 startingFrame,
                                       TrackWorkflow::TrackType trackType, bool undoRedoCommand /*= false*/ )
{
    m_tracks[trackType]->moveClip( clipUuid, oldTrack, newTrack, startingFrame, undoRedoCommand );
    computeLength();
}

Clip*       MainWorkflow::removeClip( const QUuid& uuid, unsigned int trackId, TrackWorkflow::TrackType trackType )
{
    return m_tracks[trackType]->removeClip( uuid, trackId );
}

MainWorkflow::OutputBuffers*  MainWorkflow::getSynchroneOutput()
{
    m_synchroneRenderWaitConditionMutex->lock();
    getOutput();
//    qDebug() << "Waiting for sync output";
    m_synchroneRenderWaitCondition->wait( m_synchroneRenderWaitConditionMutex );
//    qDebug() << "Got it";
    m_synchroneRenderWaitConditionMutex->unlock();
    return m_outputBuffers;
}

void        MainWorkflow::cancelSynchronisation()
{
    {
        QMutexLocker    lock( m_synchroneRenderWaitConditionMutex );
    }
    m_synchroneRenderWaitCondition->wakeAll();
}

void        MainWorkflow::muteTrack( unsigned int trackId, TrackWorkflow::TrackType trackType )
{
    m_tracks[trackType]->muteTrack( trackId );
}

void        MainWorkflow::unmuteTrack( unsigned int trackId, TrackWorkflow::TrackType trackType )
{
    m_tracks[trackType]->unmuteTrack( trackId );
}

void        MainWorkflow::setCurrentFrame( qint64 currentFrame )
{
    m_currentFrame = currentFrame;
    emit positionChanged( (float)m_currentFrame / (float)m_length );
}

Clip*       MainWorkflow::getClip( const QUuid& uuid, unsigned int trackId, TrackWorkflow::TrackType trackType )
{
    return m_tracks[trackType]->getClip( uuid, trackId );
}

void        MainWorkflow::loadProject( const QDomElement& project )
{
//    if ( project.isNull() == true || project.tagName() != "timeline" )
//    {
//        qWarning() << "Invalid timeline node (" << project.tagName() << ')';
//        return ;
//    }
//
//    clear();
//
//    QDomElement elem = project.firstChild().toElement();
//
//    while ( elem.isNull() == false )
//    {
//        bool    ok;
//
//        Q_ASSERT( elem.tagName() == "track" );
//        unsigned int trackId = elem.attribute( "id" ).toUInt( &ok );
//        if ( ok == false )
//        {
//            qWarning() << "Invalid track number in project file";
//            return ;
//        }
//        QDomElement clip = elem.firstChild().toElement();
//        while ( clip.isNull() == false )
//        {
//            //Iterate over clip fields:
//            QDomElement clipProperty = clip.firstChild().toElement();
//            QUuid       parent;
//            qint64      begin;
//            qint64      end;
//            qint64      startPos;
//
//            while ( clipProperty.isNull() == false )
//            {
//                QString tagName = clipProperty.tagName();
//                bool    ok;
//
//                if ( tagName == "parent" )
//                    parent = QUuid( clipProperty.text() );
//                else if ( tagName == "begin" )
//                {
//                    begin = clipProperty.text().toLongLong( &ok );
//                    if ( ok == false )
//                    {
//                        qWarning() << "Invalid clip begin";
//                        return ;
//                    }
//                }
//                else if ( tagName == "end" )
//                {
//                    end = clipProperty.text().toLongLong( &ok );
//                    if ( ok == false )
//                    {
//                        qWarning() << "Invalid clip end";
//                        return ;
//                    }
//                }
//                else if ( tagName == "startFrame" )
//                {
//                    startPos = clipProperty.text().toLongLong( &ok );
//                    if ( ok == false )
//                    {
//                        qWarning() << "Invalid clip starting frame";
//                        return ;
//                    }
//                }
//                else
//                    qDebug() << "Unknown field" << clipProperty.tagName();
//
//                clipProperty = clipProperty.nextSibling().toElement();
//            }
//
//            Clip*       c = new Clip( parent, begin, end );
//            addClip( c, trackId, startPos, TrackWorkflow::Video );
//
//            clip = clip.nextSibling().toElement();
//        }
//        elem = elem.nextSibling().toElement();
//    }
}

void        MainWorkflow::saveProject( QDomDocument& doc, QDomElement& rootNode )
{
//    QDomElement project = doc.createElement( "timeline" );
//    for ( unsigned int i = 0; i < m_trackCount; ++i )
//    {
//        if ( m_tracks[i]->getLength() > 0 )
//        {
//            QDomElement     trackNode = doc.createElement( "track" );
//
//            trackNode.setAttribute( "id", i );
//
//            m_tracks[i]->save( doc, trackNode );
//            project.appendChild( trackNode );
//        }
//    }
//    rootNode.appendChild( project );
}

void        MainWorkflow::clear()
{
    for ( unsigned int i = 0; i < TrackWorkflow::NbType; ++i )
        m_tracks[i]->clear();
    emit cleared();
}

void        MainWorkflow::setFullSpeedRender( bool value )
{
    for ( unsigned int i = 0; i < TrackWorkflow::NbType; ++i )
        m_tracks[i]->setFullSpeedRender( value );
}

void        MainWorkflow::tracksPaused()
{
    for ( unsigned int i = 0; i < TrackWorkflow::NbType; ++i )
        if ( m_tracks[i]->isPaused() == false )
            return ;
    emit mainWorkflowPaused();
}

void        MainWorkflow::tracksRenderCompleted()
{
    for ( unsigned int i = 0; i < TrackWorkflow::NbType; ++i )
        if ( m_tracks[i]->allTracksRendered() == false )
            return ;
    {
        QMutexLocker    lock( m_synchroneRenderWaitConditionMutex );
    }
    m_synchroneRenderWaitCondition->wakeAll();
}

int         MainWorkflow::getTrackCount( TrackWorkflow::TrackType trackType ) const
{
    return m_tracks[trackType]->getTrackCount();
}
