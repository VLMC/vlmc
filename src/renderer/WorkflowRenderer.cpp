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

WorkflowRenderer::WorkflowRenderer( MainWorkflow* mainWorkflow ) :
            m_mainWorkflow( mainWorkflow ),
            m_framePlayed( false ),
            m_pauseAsked( false ),
            m_pausedMediaPlayer( false )
{
    m_actionsLock = new QReadWriteLock;
    m_media = new LibVLCpp::Media( "fake://" );
//      --invmem-width <integer>   Width
//      --invmem-height <integer>  Height
//      --invmem-lock <string>     Lock function
//      --invmem-unlock <string>   Unlock function
//      --invmem-data <string>     Callback data
    char        buffer[64];

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

    m_mediaPlayer->setMedia( m_media );

    connect( m_mediaPlayer, SIGNAL( playing() ),    this,   SLOT( __videoPlaying() ), Qt::DirectConnection );
    connect( m_mediaPlayer, SIGNAL( paused() ),     this,   SLOT( __videoPaused() ), Qt::DirectConnection );
    connect( m_mediaPlayer, SIGNAL( stopped() ),    this,   SLOT( __videoStopped() ) );
    connect( m_mainWorkflow, SIGNAL( mainWorkflowEndReached() ), this, SLOT( __endReached() ) );
    connect( m_mainWorkflow, SIGNAL( positionChanged( float ) ), this, SLOT( __positionChanged( float ) ) );
}


WorkflowRenderer::~WorkflowRenderer()
{
    m_mediaPlayer->stop();

    disconnect( m_mediaPlayer, SIGNAL( playing() ),    this,   SLOT( __videoPlaying() ) );
    disconnect( m_mediaPlayer, SIGNAL( paused() ),     this,   SLOT( __videoPaused() ) );
    disconnect( m_mediaPlayer, SIGNAL( stopped() ),    this,   SLOT( __videoStopped() ) );
    disconnect( m_mainWorkflow, SIGNAL( mainWorkflowEndReached() ), this, SLOT( __endReached() ) );
    disconnect( m_mainWorkflow, SIGNAL( positionChanged( float ) ), this, SLOT( __positionChanged( float ) ) );

    delete m_actionsLock;
    delete m_media;
}

void*   WorkflowRenderer::lock( void* datas )
{
    WorkflowRenderer* self = reinterpret_cast<WorkflowRenderer*>( datas );

    //If renderer is stopping, don't ask for another frame:
    if ( self->m_isRendering == false )
    {
        qDebug() << "WorkflowRenderer doesn't render anymore (stopping state)";
        return self->m_lastFrame;
    }
    //If a pause was asked, don't try to start a new render... it could (and would) dead lock...
    if ( self->m_pauseAsked == true )
    {
        qDebug() << "WorkflowRenderer is paused, returning last frame";
        return self->m_lastFrame;
    }
    //If we're not playing, then where in a paused media player.
    if ( self->m_pausedMediaPlayer == true )
    {
        qDebug() << "Returning last frame, since media player is paused";
        return self->m_lastFrame;
    }
    if ( self->m_oneFrameOnly < 2 )
    {
        void* ret;
//        if ( self->m_oneFrameOnly == 0 )
//            ret = self->m_mainWorkflow->getOutput();
//        else
//        {
            qDebug() << "Asking synchrone frame";
            ret = self->m_mainWorkflow->getSynchroneOutput();
            qDebug() << "Got it";
//        }
        self->m_lastFrame = static_cast<unsigned char*>( ret );
        qDebug() << "Returning new frame";
        return ret;
    }
    else
    {
        qDebug() << "Returning last frame, due to m_oneFrameOnly flag (" << self->m_oneFrameOnly << ")";
        return self->m_lastFrame;
    }
}

void    WorkflowRenderer::unlock( void* datas )
{
    WorkflowRenderer* self = reinterpret_cast<WorkflowRenderer*>( datas );

//    qDebug() << "Workflowrenderer::unlock. m_oneFrameOnly ==" << self->m_oneFrameOnly;
    if ( self->m_oneFrameOnly == 1 )
    {
//        qDebug() << "Pausing back";
        self->togglePlayPause( true );
//        qDebug() << "Switching m_oneFrameOnly flag to 2";
        self->m_oneFrameOnly = 2;
    }
    self->m_framePlayed = true;
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
                m_mediaPlayer->pause();
                //This will also pause the MainWorkflow via a signal/slot
                break ;
            default:
                qDebug() << "Unhandled action:" << act;
                break ;
        }
    }
}

void        WorkflowRenderer::stopPreview()
{
    disconnect( m_mainWorkflow, SIGNAL( frameChanged(qint64) ),
             Timeline::getInstance()->tracksView()->tracksCursor(), SLOT( updateCursorPos( qint64 ) ) );
    stop();
}

void        WorkflowRenderer::startPreview()
{
    char        buff[128];

    connect( m_mainWorkflow, SIGNAL( frameChanged(qint64) ),
             Timeline::getInstance()->tracksView()->tracksCursor(), SLOT( updateCursorPos( qint64 ) ) );
    connect( m_mainWorkflow, SIGNAL( mainWorkflowPaused() ), this, SLOT( mainWorkflowPaused() ) );
    m_mainWorkflow->startRender();
    sprintf( buff, ":fake-duration=%lli", m_mainWorkflow->getLength() / FPS * 1000 );
    m_media->addOption( buff );
    m_mediaPlayer->play();
    m_isRendering = true;
    m_paused = false;
}

void        WorkflowRenderer::setPosition( float newPos )
{
    m_mainWorkflow->setPosition( newPos );
}

void        WorkflowRenderer::frameByFrameAfterPaused()
{
    qDebug() << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<";

    m_oneFrameOnly = 1;
    m_mainWorkflow->activateOneFrameOnly();

//    m_mainWorkflow->nextFrame();

    togglePlayPause( false );
}

void        WorkflowRenderer::frameByFramePausingProxy()
{
    static unsigned int nbPaused = 0;
    ++nbPaused;
    if ( nbPaused == 2 )
    {
        nbPaused = 0;
        frameByFrameAfterPaused();
    }
}

void        WorkflowRenderer::nextFrame()
{
    if ( m_pausedMediaPlayer == true )
    {
        frameByFrameAfterPaused();
    }
    else
    {
        connect( m_mediaPlayer, SIGNAL( paused() ), this, SLOT( frameByFramePausingProxy() ), Qt::QueuedConnection );
        connect( m_mainWorkflow, SIGNAL( mainWorkflowPaused() ), this, SLOT( frameByFramePausingProxy() ), Qt::QueuedConnection );
        togglePlayPause( true );
    }
}

void        WorkflowRenderer::previousFrame()
{

}

void        WorkflowRenderer::pauseMainWorkflow()
{
    if ( m_paused == true )
        return ;
    m_pausedMediaPlayer = true;
    m_mainWorkflow->pause();
}

void        WorkflowRenderer::mainWorkflowPaused()
{
    m_paused = true;
    m_pauseAsked = false;
    emit paused();
}

void        WorkflowRenderer::togglePlayPause( bool forcePause )
{
    //If force pause is true, we just ensure that this render is paused... no need to start it.
    if ( m_isRendering == false && forcePause == false )
        startPreview();
    else if ( m_isRendering == true )
    {
        if ( m_paused == true && forcePause == false )
        {
            //This will automaticly unpause the ClipWorkflow... no worries
            m_mediaPlayer->play();
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
    m_mainWorkflow->cancelSynchronisation();
    m_mediaPlayer->stop();
    m_mainWorkflow->stop();
    m_oneFrameOnly = 0;
}

/////////////////////////////////////////////////////////////////////
/////SLOTS :
/////////////////////////////////////////////////////////////////////

void        WorkflowRenderer::__endReached()
{
    stopPreview();
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

void        WorkflowRenderer::__videoPaused()
{
    if ( m_oneFrameOnly != 0 )
    {
        m_oneFrameOnly = 0;
    }
    pauseMainWorkflow();
}

void        WorkflowRenderer::__videoPlaying()
{
    emit playing();
    m_pausedMediaPlayer = false;
    m_paused = false;
}

void        WorkflowRenderer::__videoStopped()
{
    emit endReached();
}
