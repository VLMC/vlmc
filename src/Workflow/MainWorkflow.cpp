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

#include "vlmc.h"
#include "MainWorkflow.h"
#include "TrackWorkflow.h"
#include "TrackHandler.h"
#include "Library.h"
#include "SettingsManager.h"

LightVideoFrame*    MainWorkflow::blackOutput = NULL;

MainWorkflow::MainWorkflow( int trackCount ) :
        m_lengthFrame( 0 ),
        m_renderStarted( false )
{
    m_currentFrameLock = new QReadWriteLock;
    m_renderStartedLock = new QReadWriteLock;
    m_renderMutex = new QMutex;

    const SettingValue* width = SettingsManager::getInstance()->getValue( "project", "VideoProjectWidth" );
    connect( width, SIGNAL( changed( QVariant ) ), this, SLOT( widthChanged( QVariant ) ) );
    m_width = width->get().toUInt();
    const SettingValue* height = SettingsManager::getInstance()->getValue( "project", "VideoProjectHeight" );
    connect( height, SIGNAL( changed( QVariant ) ), this, SLOT( widthChanged( QVariant ) ) );
    m_height = height->get().toUInt();

    m_effectEngine = new EffectsEngine;
    m_effectEngine->disable();

    m_tracks = new TrackHandler*[MainWorkflow::NbTrackType];
    m_currentFrame = new qint64[MainWorkflow::NbTrackType];
    for ( unsigned int i = 0; i < MainWorkflow::NbTrackType; ++i )
    {
        MainWorkflow::TrackType trackType = (i == 0 ? MainWorkflow::VideoTrack : MainWorkflow::AudioTrack );
        m_tracks[i] = new TrackHandler( trackCount, trackType, m_effectEngine );
        connect( m_tracks[i], SIGNAL( tracksEndReached() ), this, SLOT( tracksEndReached() ) );
	m_currentFrame[i] = 0;
    }
    m_outputBuffers = new OutputBuffers;

    blackOutput = new LightVideoFrame( m_width * m_height * Pixel::NbComposantes );
    memset( (*blackOutput)->frame.octets, 0, (*blackOutput)->nboctets );
}

MainWorkflow::~MainWorkflow()
{
    //FIXME: this is probably useless, since already done by the renderer
    stop();

    delete m_effectEngine;
    delete m_renderMutex;
    delete m_renderStartedLock;
    delete m_currentFrameLock;
    delete m_currentFrame;
    for ( unsigned int i = 0; i < MainWorkflow::NbTrackType; ++i )
        delete m_tracks[i];
    delete[] m_tracks;
}

EffectsEngine*          MainWorkflow::getEffectsEngine()
{
    return m_effectEngine;
}

void            MainWorkflow::addClip( Clip* clip, unsigned int trackId,
                                        qint64 start, MainWorkflow::TrackType trackType )
{
    m_tracks[trackType]->addClip( clip, trackId, start );
    computeLength();
    //Inform the GUI
    emit clipAdded( clip, trackId, start, trackType );
}

void            MainWorkflow::computeLength()
{
    qint64      maxLength = 0;

    for ( unsigned int i = 0; i < MainWorkflow::NbTrackType; ++i )
    {
        if ( m_tracks[i]->getLength() > maxLength )
            maxLength = m_tracks[i]->getLength();
    }
    m_lengthFrame = maxLength;
}

void    MainWorkflow::startRender()
{
    m_renderStarted = true;
    m_paused = false;
    for ( unsigned int i = 0; i < MainWorkflow::NbTrackType; ++i )
        m_tracks[i]->startRender();
    computeLength();
}

MainWorkflow::OutputBuffers*    MainWorkflow::getOutput( TrackType trackType )
{
    QReadLocker         lock( m_renderStartedLock );
    QMutexLocker        lock2( m_renderMutex );

    if ( m_renderStarted == true )
    {
        QReadLocker         lock3( m_currentFrameLock );

        m_tracks[trackType]->getOutput( m_currentFrame[VideoTrack], m_currentFrame[trackType] );
        if ( trackType == MainWorkflow::VideoTrack )
        {
            m_effectEngine->render();
            if ( m_effectEngine->getOutputFrame( 0 )->nboctets == 0 )
                m_outputBuffers->video = MainWorkflow::blackOutput;
            else
                m_outputBuffers->video = &( m_effectEngine->getOutputFrame( 0 ) );
        }
        else
        {
            m_outputBuffers->audio = m_tracks[MainWorkflow::AudioTrack]->getTmpAudioBuffer();
        }
    }
    return m_outputBuffers;
}

void        MainWorkflow::pause()
{
    {
        //Just wait for the current render to finish
        //TODO:
        //FIXME: check if this is not alreay handled by the stacked actions system.
        QMutexLocker    lock( m_renderMutex );

        for ( unsigned int i = 0; i < MainWorkflow::NbTrackType; ++i )
            m_tracks[i]->pause();
    }
    emit mainWorkflowPaused();
}

void        MainWorkflow::unpause()
{
    {
        QMutexLocker    lock( m_renderMutex );

        for ( unsigned int i = 0; i < MainWorkflow::NbTrackType; ++i )
            m_tracks[i]->unpause();
    }
    emit mainWorkflowUnpaused();
}

void        MainWorkflow::goToNextFrame( MainWorkflow::TrackType trackType )
{
    if ( m_paused == false )
        nextFrame( trackType );
}

void        MainWorkflow::nextFrame( MainWorkflow::TrackType trackType )
{
    QWriteLocker    lock( m_currentFrameLock );

    ++m_currentFrame[trackType];
    if ( trackType == MainWorkflow::VideoTrack )
        emit frameChanged( m_currentFrame[MainWorkflow::VideoTrack], Renderer );
}

void        MainWorkflow::previousFrame( MainWorkflow::TrackType trackType )
{
    QWriteLocker    lock( m_currentFrameLock );

    --m_currentFrame[trackType];
    if ( trackType == MainWorkflow::VideoTrack )
        emit frameChanged( m_currentFrame[MainWorkflow::VideoTrack], Renderer );
}

qint64      MainWorkflow::getLengthFrame() const
{
    return m_lengthFrame;
}

qint64      MainWorkflow::getClipPosition( const QUuid& uuid, unsigned int trackId, MainWorkflow::TrackType trackType ) const
{
    return m_tracks[trackType]->getClipPosition( uuid, trackId );
}

void            MainWorkflow::stop()
{
    QWriteLocker    lock( m_renderStartedLock );
    QWriteLocker    lock2( m_currentFrameLock );

    m_renderStarted = false;
    for (unsigned int i = 0; i < MainWorkflow::NbTrackType; ++i)
    {
        m_tracks[i]->stop();
        m_currentFrame[i] = 0;
    }
    emit frameChanged( 0, Renderer );
}

void           MainWorkflow::moveClip( const QUuid& clipUuid, unsigned int oldTrack,
                                       unsigned int newTrack, qint64 startingFrame,
                                       MainWorkflow::TrackType trackType, bool undoRedoCommand /*= false*/ )
{
    m_tracks[trackType]->moveClip( clipUuid, oldTrack, newTrack, startingFrame );
    computeLength();

    if ( undoRedoCommand == true )
    {
        emit clipMoved( clipUuid, newTrack, startingFrame, trackType );
    }
}

Clip*       MainWorkflow::removeClip( const QUuid& uuid, unsigned int trackId, MainWorkflow::TrackType trackType )
{
    Clip* clip = m_tracks[trackType]->removeClip( uuid, trackId );
    computeLength();
    emit clipRemoved( clip, trackId, trackType );
    return clip;
}

void        MainWorkflow::muteTrack( unsigned int trackId, MainWorkflow::TrackType trackType )
{
    m_tracks[trackType]->muteTrack( trackId );
}

void        MainWorkflow::unmuteTrack( unsigned int trackId, MainWorkflow::TrackType trackType )
{
    m_tracks[trackType]->unmuteTrack( trackId );
}

void        MainWorkflow::setCurrentFrame( qint64 currentFrame, MainWorkflow::FrameChangedReason reason )
{
    QWriteLocker    lock( m_currentFrameLock );

    if ( m_renderStarted == true )
    {
        //Since any track can be reactivated, we reactivate all of them, and let them
        //unable themself if required.
        for ( unsigned int i = 0; i < MainWorkflow::NbTrackType; ++i)
            m_tracks[i]->activateAll();
    }
    for ( unsigned int i = 0; i < MainWorkflow::NbTrackType; ++i)
        m_currentFrame[i] = currentFrame;
    emit frameChanged( currentFrame, reason );
}

Clip*       MainWorkflow::getClip( const QUuid& uuid, unsigned int trackId, MainWorkflow::TrackType trackType )
{
    return m_tracks[trackType]->getClip( uuid, trackId );
}

/**
 *  \warning    The mainworkflow is expected to be cleared already by the ProjectManager
 */
void        MainWorkflow::loadProject( const QDomElement& project )
{
    if ( project.isNull() == true || project.tagName() != "timeline" )
    {
        qWarning() << "Invalid timeline node (" << project.tagName() << ')';
        return ;
    }

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
            QUuid                       parent;
            qint64                      begin;
            qint64                      end;
            qint64                      startPos;
            MainWorkflow::TrackType     trackType = MainWorkflow::VideoTrack;

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
                else if ( tagName == "trackType" )
                {
                    trackType = static_cast<MainWorkflow::TrackType>( clipProperty.text().toUInt( &ok ) );
                    if ( ok == false )
                    {
                        qWarning() << "Invalid track type starting frame";
                        return ;
                    }
                }
                else
                    qDebug() << "Unknown field" << clipProperty.tagName();

                clipProperty = clipProperty.nextSibling().toElement();
            }

            if ( Library::getInstance()->getMedia( parent ) != NULL )
            {
                Clip*       c = new Clip( parent, begin, end );
                addClip( c, trackId, startPos, trackType );
            }

            clip = clip.nextSibling().toElement();
        }
        elem = elem.nextSibling().toElement();
    }
}

void        MainWorkflow::saveProject( QDomDocument& doc, QDomElement& rootNode )
{
    QDomElement project = doc.createElement( "timeline" );
    for ( unsigned int i = 0; i < MainWorkflow::NbTrackType; ++i )
    {
        m_tracks[i]->save( doc, project );
    }
    rootNode.appendChild( project );
}

void        MainWorkflow::clear()
{
    for ( unsigned int i = 0; i < MainWorkflow::NbTrackType; ++i )
        m_tracks[i]->clear();
    emit cleared();
}

void        MainWorkflow::tracksEndReached()
{
    for ( unsigned int i = 0; i < MainWorkflow::NbTrackType; ++i )
        if ( m_tracks[i]->endIsReached() == false )
            return ;
    emit mainWorkflowEndReached();
}

int         MainWorkflow::getTrackCount( MainWorkflow::TrackType trackType ) const
{
    return m_tracks[trackType]->getTrackCount();
}

qint64      MainWorkflow::getCurrentFrame() const
{
    QReadLocker     lock( m_currentFrameLock );

    return m_currentFrame[MainWorkflow::VideoTrack];
}

void        MainWorkflow::widthChanged( const QVariant& width )
{
    m_width = width.toUInt();
}

void        MainWorkflow::heightChanged( const QVariant& height )
{
    m_height = height.toUInt();
}

uint32_t    MainWorkflow::getWidth() const
{
    return m_width;
}

uint32_t    MainWorkflow::getHeight() const
{
    return m_height;
}
