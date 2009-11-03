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
#include "Timeline.h"

//FIXME: remove this...
#define OUTPUT_FPS  30

WorkflowRenderer::WorkflowRenderer() :
            m_mainWorkflow( MainWorkflow::getInstance() ),
            m_stopping( false ),
            m_pauseAsked( false ),
            m_unpauseAsked( false )
{
    char        buffer[64];

    m_actionsLock = new QReadWriteLock;
    m_media = new LibVLCpp::Media( "fake://" );

    sprintf( buffer, ":invmem-width=%i", VIDEOWIDTH );
    m_media->addOption( ":codec=invmem" );
    m_media->addOption( buffer );
    sprintf( buffer, ":invmem-height=%i", VIDEOHEIGHT );
    m_media->addOption( buffer );
    sprintf( buffer, ":invmem-lock=%lld", (qint64)WorkflowRenderer::lock );
    m_media->addOption( buffer );
    sprintf( buffer, ":invmem-unlock=%lld", (qint64)WorkflowRenderer::unlock );
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

    m_condMutex = new QMutex;
    m_waitCond = new QWaitCondition;

    m_renderVideoFrame = new unsigned char[VIDEOHEIGHT * VIDEOWIDTH * Pixel::NbComposantes];

     //Workflow part
    connect( m_mainWorkflow, SIGNAL( frameChanged(qint64) ),
            Timeline::getInstance()->tracksView()->tracksCursor(), SLOT( setCursorPos( qint64 ) ), Qt::QueuedConnection );
    connect( Timeline::getInstance()->tracksView()->tracksCursor(), SIGNAL( cursorPositionChanged( qint64 ) ),
             this, SLOT( timelineCursorChanged(qint64) ) );
    connect( m_mainWorkflow, SIGNAL( mainWorkflowPaused() ), this, SLOT( mainWorkflowPaused() ) );
    connect( m_mainWorkflow, SIGNAL( mainWorkflowUnpaused() ), this, SLOT( mainWorkflowUnpaused() ) );

}


WorkflowRenderer::~WorkflowRenderer()
{
    stop();

    //FIXME this is probably useless...
    //etix says: yes it is...
    disconnect( m_mediaPlayer, SIGNAL( playing() ),    this,   SLOT( __videoPlaying() ) );
    disconnect( m_mediaPlayer, SIGNAL( paused() ),     this,   SLOT( __videoPaused() ) );
    disconnect( m_mediaPlayer, SIGNAL( stopped() ),    this,   SLOT( __videoStopped() ) );
    disconnect( m_mainWorkflow, SIGNAL( mainWorkflowEndReached() ), this, SLOT( __endReached() ) );
    disconnect( m_mainWorkflow, SIGNAL( positionChanged( float ) ), this, SLOT( __positionChanged( float ) ) );
    disconnect( m_mainWorkflow, SIGNAL( frameChanged( qint64 ) ), this, SLOT( __frameChanged( qint64 ) ) );

    delete m_actionsLock;
    delete m_media;
    delete m_condMutex;
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

    if ( self->m_stopping == false )
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
    QReadLocker     lock( m_actionsLock );

    if ( m_actions.size() == 0 )
        return ;
    while ( m_actions.empty() == false )
    {
        Actions act = m_actions.top();
        m_actions.pop();
        switch ( act )
        {
            case    Pause:
                if ( m_pauseAsked == true )
                    continue ;
                m_pauseAsked = true;
//                m_mediaPlayer->pause();
                pauseMainWorkflow();
                //This will also pause the MainWorkflow via a signal/slot
                break ;
            default:
                qDebug() << "Unhandled action:" << act;
                break ;
        }
    }
}

void        WorkflowRenderer::startPreview()
{
    if ( m_mainWorkflow->getLengthFrame() <= 0 )
        return ;
    m_mediaPlayer->setMedia( m_media );

    //Media player part: to update PreviewWidget
    connect( m_mediaPlayer, SIGNAL( playing() ),    this,   SLOT( __videoPlaying() ), Qt::DirectConnection );
    connect( m_mediaPlayer, SIGNAL( paused() ),     this,   SLOT( __videoPaused() ), Qt::DirectConnection );
    connect( m_mediaPlayer, SIGNAL( stopped() ),    this,   SLOT( __videoStopped() ) );
    connect( m_mainWorkflow, SIGNAL( mainWorkflowEndReached() ), this, SLOT( __endReached() ) );
    connect( m_mainWorkflow, SIGNAL( positionChanged( float ) ), this, SLOT( __positionChanged( float ) ) );
    connect( m_mainWorkflow, SIGNAL( frameChanged( qint64 ) ), this, SLOT( __frameChanged( qint64 ) ) );

    m_mainWorkflow->setFullSpeedRender( false );
    m_mainWorkflow->startRender();
    m_mediaPlayer->play();
    m_isRendering = true;
    m_paused = false;
    m_stopping = false;
}

void        WorkflowRenderer::setPosition( float newPos )
{
    m_mainWorkflow->setPosition( newPos );
}

void        WorkflowRenderer::nextFrame()
{
    m_mainWorkflow->nextFrame();
}

void        WorkflowRenderer::previousFrame()
{
    m_mainWorkflow->previousFrame();
}

void        WorkflowRenderer::pauseMainWorkflow()
{
    if ( m_paused == true )
        return ;

    QMutexLocker    lock( m_condMutex );
    m_mainWorkflow->pause();
    m_waitCond->wait( m_condMutex );
}

void        WorkflowRenderer::unpauseMainWorkflow()
{
    if ( m_paused == false )
        return ;
    m_mainWorkflow->unpause();
}

void        WorkflowRenderer::mainWorkflowPaused()
{
    m_paused = true;
    m_pauseAsked = false;
    {
        QMutexLocker    lock( m_condMutex );
    }
    m_waitCond->wakeAll();
    emit paused();
}

void        WorkflowRenderer::mainWorkflowUnpaused()
{
    m_paused = false;
    m_unpauseAsked = false;
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
            if ( m_paused == true )
            {
                m_unpauseAsked = true;
                unpauseMainWorkflow();
            }
        }
        else
        {
            if ( m_paused == false )
            {
                QWriteLocker        lock( m_actionsLock );
                m_actions.push( Pause );
            }
        }
    }
}

void        WorkflowRenderer::stop()
{
    m_isRendering = false;
    m_paused = false;
    m_pauseAsked = false;
    m_unpauseAsked = false;
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
    return m_mainWorkflow->getLengthFrame() * OUTPUT_FPS * 1000;
}

float       WorkflowRenderer::getFps() const
{
    //Sigh :'(
    return static_cast<float>( OUTPUT_FPS );
}

/////////////////////////////////////////////////////////////////////
/////SLOTS :
/////////////////////////////////////////////////////////////////////

void        WorkflowRenderer::__endReached()
{
    stop();
    emit endReached();
}

void        WorkflowRenderer::__positionChanged()
{
    qFatal("This should never be used ! Get out of here !");
}

void        WorkflowRenderer::__positionChanged( float pos )
{
    emit positionChanged( pos );
}

void        WorkflowRenderer::__frameChanged( qint64 frame )
{
    emit frameChanged( frame );
}

void        WorkflowRenderer::__videoPaused()
{
    if ( m_pauseAsked == true )
        pauseMainWorkflow();
}

void        WorkflowRenderer::__videoPlaying()
{
    if ( m_unpauseAsked == true )
        unpauseMainWorkflow();
    else
    {
        m_paused = false;
        emit playing();
    }
}

void        WorkflowRenderer::__videoStopped()
{
    emit endReached();
}

void        WorkflowRenderer::timelineCursorChanged( qint64 newFrame )
{
    m_mainWorkflow->setCurrentFrame( newFrame );
}

