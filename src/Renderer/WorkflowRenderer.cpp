/*****************************************************************************
 * WorkflowRenderer.cpp: Allow a current workflow preview
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
#include <QThread>

#include "vlmc.h"
#include "WorkflowRenderer.h"
#include "timeline/Timeline.h"
#include "SettingsManager.h"

WorkflowRenderer::WorkflowRenderer() :
            m_mainWorkflow( MainWorkflow::getInstance() ),
            m_stopping( false )
{
    m_actionsMutex = new QMutex;
}

void    WorkflowRenderer::initializeRenderer()
{
    char        buffer[64];
    m_media = new LibVLCpp::Media( "fake://" );

    sprintf( buffer, ":invmem-width=%i", VIDEOWIDTH );
    m_media->addOption( ":codec=invmem" );
    m_media->addOption( buffer );
    sprintf( buffer, ":invmem-height=%i", VIDEOHEIGHT );
    m_media->addOption( buffer );
    sprintf( buffer, ":invmem-lock=%lld", (qint64)getLockCallback() );
    m_media->addOption( buffer );
    sprintf( buffer, ":invmem-unlock=%lld", (qint64)getUnlockCallback() );
    m_media->addOption( buffer );
    sprintf( buffer, ":invmem-data=%lld", (qint64)this );
    m_media->addOption( buffer );
    sprintf( buffer, ":width=%i", VIDEOWIDTH );
    m_media->addOption( buffer );
    sprintf( buffer, ":height=%i", VIDEOHEIGHT );
    m_media->addOption( buffer );

    m_media->addOption( ":no-audio" );
//    sprintf( buffer, ":inamem-data=%lld", (qint64)this );
//    m_media->addOption( buffer );
//    sprintf( buffer, ":inamem-callback=%lld", (qint64)WorkflowRenderer::lock );
//    m_media->addOption( buffer );

    m_waitCond = new QWaitCondition;

    m_renderVideoFrame = new unsigned char[VIDEOHEIGHT * VIDEOWIDTH * Pixel::NbComposantes];

     //Workflow part
    connect( m_mainWorkflow, SIGNAL( mainWorkflowPaused() ), this, SLOT( mainWorkflowPaused() ), Qt::QueuedConnection );
    connect( m_mainWorkflow, SIGNAL( mainWorkflowUnpaused() ), this, SLOT( mainWorkflowUnpaused() ), Qt::QueuedConnection );
    connect( m_mainWorkflow, SIGNAL( mainWorkflowEndReached() ), this, SLOT( __endReached() ) );
    connect( m_mainWorkflow, SIGNAL( frameChanged( qint64, MainWorkflow::FrameChangedReason ) ),
             this, SIGNAL( frameChanged( qint64, MainWorkflow::FrameChangedReason ) ) );
}

WorkflowRenderer::~WorkflowRenderer()
{
    stop();

    delete m_actionsMutex;
    delete m_media;
    delete m_waitCond;
}

void*   WorkflowRenderer::lockAudio( void* datas )
{
    WorkflowRenderer* self = reinterpret_cast<WorkflowRenderer*>( datas );

    qDebug() << "Injecting audio data";
    return self->m_renderAudioSample;
}

void*   WorkflowRenderer::lock( void* datas )
{
    WorkflowRenderer* self = reinterpret_cast<WorkflowRenderer*>( datas );

    if ( self->m_stopping == false && self->m_paused == false )
    {
        MainWorkflow::OutputBuffers* ret = self->m_mainWorkflow->getSynchroneOutput();
        memcpy( self->m_renderVideoFrame, (*(ret->video))->frame.octets, (*(ret->video))->nboctets );
        self->m_renderAudioSample = ret->audio;
    }
    return self->m_renderVideoFrame;
}

void    WorkflowRenderer::unlock( void* datas )
{
    WorkflowRenderer* self = reinterpret_cast<WorkflowRenderer*>( datas );
    self->checkActions();
}

void        WorkflowRenderer::checkActions()
{
    QMutexLocker    lock( m_actionsMutex );

    if ( m_actions.size() == 0 )
        return ;
    while ( m_actions.empty() == false )
    {
        Action::Generic*   act = m_actions.top();
        m_actions.pop();
        act->execute();
        delete act;
    }
}

void        WorkflowRenderer::startPreview()
{
    if ( m_mainWorkflow->getLengthFrame() <= 0 )
        return ;
    m_mediaPlayer->setMedia( m_media );

    //Media player part: to update PreviewWidget
    connect( m_mediaPlayer, SIGNAL( playing() ),    this,   SIGNAL( playing() ), Qt::DirectConnection );
    connect( m_mediaPlayer, SIGNAL( paused() ),     this,   SIGNAL( paused() ), Qt::DirectConnection );
    //FIXME:: check if this doesn't require Qt::QueuedConnection
    connect( m_mediaPlayer, SIGNAL( stopped() ),    this,   SIGNAL( endReached() ) );

    m_mainWorkflow->setFullSpeedRender( false );
    m_mainWorkflow->startRender();
    m_isRendering = true;
    m_paused = false;
    m_stopping = false;
    m_outputFps = SettingsManager::getInstance()->getValue( "VLMC", "VLMCOutPutFPS" )->get().toDouble();
    m_mediaPlayer->play();
}

void        WorkflowRenderer::nextFrame()
{
    m_mainWorkflow->nextFrame();
}

void        WorkflowRenderer::previousFrame()
{
    m_mainWorkflow->previousFrame();
}

void        WorkflowRenderer::mainWorkflowPaused()
{
    m_paused = true;
    emit paused();
}

void        WorkflowRenderer::mainWorkflowUnpaused()
{
    m_paused = false;
    emit playing();
}

void        WorkflowRenderer::togglePlayPause( bool forcePause )
{
    if ( m_isRendering == false && forcePause == false )
        startPreview();
    else
        internalPlayPause( forcePause );
}

void        WorkflowRenderer::internalPlayPause( bool forcePause )
{
    //If force pause is true, we just ensure that this render is paused... no need to start it.
    if ( m_isRendering == true )
    {
        if ( m_paused == true && forcePause == false )
        {
            Action::Generic*    act = new Action::Unpause( m_mainWorkflow );
            QMutexLocker        lock( m_actionsMutex );
            m_actions.addAction( act );
        }
        else
        {
            if ( m_paused == false )
            {
                Action::Generic*    act = new Action::Pause( m_mainWorkflow );
                QMutexLocker        lock( m_actionsMutex );
                m_actions.addAction( act );
            }
        }
    }
}

void        WorkflowRenderer::stop()
{
    m_isRendering = false;
    m_paused = false;
    m_stopping = true;
    m_mainWorkflow->cancelSynchronisation();
    m_mediaPlayer->stop();
    m_mainWorkflow->stop();
}

qint64      WorkflowRenderer::getCurrentFrame() const
{
    return m_mainWorkflow->getCurrentFrame();
}

qint64      WorkflowRenderer::getLengthMs() const
{
    return m_mainWorkflow->getLengthFrame() / getFps() * 1000;
}

float       WorkflowRenderer::getFps() const
{
    return m_outputFps;
}

void        WorkflowRenderer::removeClip( const QUuid& uuid, uint32_t trackId, MainWorkflow::TrackType trackType )
{
    if ( m_isRendering == true )
    {
        Action::Generic*    act = new Action::RemoveClip( m_mainWorkflow, trackId, trackType, uuid );
        QMutexLocker        lock( m_actionsMutex );
        m_actions.addAction( act );
    }
    else
        m_mainWorkflow->removeClip( uuid, trackId, trackType );
}

void        WorkflowRenderer::addClip( Clip* clip, uint32_t trackId, qint64 startingPos, MainWorkflow::TrackType trackType )
{
    if ( m_isRendering == true )
    {
        Action::Generic*    act = new Action::AddClip( m_mainWorkflow, trackId, trackType, clip, startingPos );
        QMutexLocker        lock( m_actionsMutex );
        m_actions.addAction( act );
    }
    else
        m_mainWorkflow->addClip( clip, trackId, startingPos, trackType );
}

void        WorkflowRenderer::timelineCursorChanged( qint64 newFrame )
{
    m_mainWorkflow->setCurrentFrame( newFrame, MainWorkflow::TimelineCursor );
}

void        WorkflowRenderer::previewWidgetCursorChanged( qint64 newFrame )
{
    m_mainWorkflow->setCurrentFrame( newFrame, MainWorkflow::PreviewCursor );
}

void        WorkflowRenderer::rulerCursorChanged( qint64 newFrame )
{
    m_mainWorkflow->setCurrentFrame( newFrame, MainWorkflow::RulerCursor );
}

Clip*       WorkflowRenderer::split( Clip* toSplit, Clip* newClip, uint32_t trackId, qint64 newClipPos, qint64 newClipBegin, MainWorkflow::TrackType trackType )
{
    if ( newClip == NULL )
        newClip = new Clip( toSplit, newClipBegin, toSplit->getEnd() );

    if ( m_isRendering == true )
    {
        //adding clip
        //We can NOT call addClip, as it would lock the action lock and then release it,
        //thus potentially breaking the synchrone way of doing this
        Action::Generic*    act = new Action::AddClip( m_mainWorkflow, trackId, trackType, newClip, newClipPos );
        //resizing it
        Action::Generic*    act2 = new Action::ResizeClip( toSplit, toSplit->getBegin(), newClipBegin, true );

        //Push the actions onto the action stack
        QMutexLocker    lock( m_actionsMutex );
        m_actions.addAction( act );
        m_actions.addAction( act2 );
    }
    else
    {
        toSplit->setEnd( newClipBegin, true );
        m_mainWorkflow->addClip( newClip, trackId, newClipPos, trackType );
    }
    return newClip;
}

void    WorkflowRenderer::unsplit( Clip* origin, Clip* splitted, uint32_t trackId, MainWorkflow::TrackType trackType )
{
    if ( m_isRendering == true )
    {
        //removing clip
        Action::Generic*    act = new Action::RemoveClip( m_mainWorkflow, trackId, trackType, splitted->getUuid() );
        //resizing it
        Action::Generic*    act2 = new Action::ResizeClip( origin, origin->getBegin(), splitted->getEnd(), true );
        //Push the actions onto the action stack
        QMutexLocker        lock( m_actionsMutex );
        m_actions.addAction( act );
        m_actions.addAction( act2 );
    }
    else
    {
        m_mainWorkflow->removeClip( splitted->getUuid(), trackId, trackType );
        origin->setEnd( splitted->getEnd(), true );
    }
}

void    WorkflowRenderer::resizeClip( Clip* clip, qint64 newBegin, qint64 newEnd,
                                      qint64 newPos, uint32_t trackId, MainWorkflow::TrackType trackType, bool undoRedoAction /*= false*/ )
{
//    if ( m_isRendering == true )
//    {
//        Action::Generic*    act = new Action::ResizeClip( clip, newBegin, newEnd );
//        Action::Generic*    act2 = new Action::MoveClip( m_mainWorkflow, clip->getUuid(), trackId, trackId, newPos, trackType, undoRedoAction );
//        QMutexLocker        lock( m_actionsMutex );
//        if ( newBegin != clip->getBegin() )
//        {
//            qDebug() << "Resizing to pos:" << newPos;
//            m_actions.addAction( act2 );
//        }
//        qDebug() << "setting boundaries: newEnd:" << newBegin << "newEnd:" << newEnd;
//        m_actions.addAction( act );
//    }
//    else
//    {
        if ( newBegin != clip->getBegin() )
        {
            m_mainWorkflow->moveClip( clip->getUuid(), trackId, trackId, newPos, trackType, undoRedoAction );
        }
        clip->setBoundaries( newBegin, newEnd );

//    }
}

void*   WorkflowRenderer::getLockCallback()
{
    return (void*)&WorkflowRenderer::lock;
}

void*   WorkflowRenderer::getUnlockCallback()
{
    return (void*)&WorkflowRenderer::unlock;
}

/////////////////////////////////////////////////////////////////////
/////SLOTS :
/////////////////////////////////////////////////////////////////////

void        WorkflowRenderer::__endReached()
{
    stop();
    emit endReached();
}

