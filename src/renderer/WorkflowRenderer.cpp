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
            m_pauseAsked( false )
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

    connect( m_mediaPlayer, SIGNAL( playing() ),    this,   SLOT( __videoPlaying() ) );
    connect( m_mediaPlayer, SIGNAL( paused() ),     this,   SLOT( __videoPaused() ) );
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
    delete m_mainWorkflow;
}

void*   WorkflowRenderer::lock( void* datas )
{
    WorkflowRenderer* self = reinterpret_cast<WorkflowRenderer*>( datas );

    //If we're not playing, then where in a paused media player.
    if ( self->m_mediaPlayer->isPlaying() == false )
        return self->m_lastFrame;
    if ( self->m_oneFrameOnly < 2 )
    {
        qDebug() << "\nQuerying new picture";
        void* ret = self->m_mainWorkflow->getOutput();
        self->m_lastFrame = static_cast<unsigned char*>( ret );
        qDebug() << "Got picture";
        return ret;
    }
    else
        return self->m_lastFrame;
}

void    WorkflowRenderer::unlock( void* datas )
{
    WorkflowRenderer* self = reinterpret_cast<WorkflowRenderer*>( datas );
    if ( self->m_oneFrameOnly == 1 )
    {
        self->m_mediaPlayer->pause();
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
                qDebug() << "Pausing WorkflowRenderer mediaPlayer";
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

void        WorkflowRenderer::nextFrame()
{
//    qDebug() << "Next frame :";
    m_oneFrameOnly = 1;
    m_mainWorkflow->nextFrame();
//    qDebug() << "Activatign one frame only";
    m_mainWorkflow->activateOneFrameOnly();
    //Both media players should be stopped now... restauring playback
//    m_framePlayed = 0;
    m_mediaPlayer->pause();
//    while ( m_framePlayed == 0 )
//        SleepMS( 1 );
//    m_mediaPlayer->pause();
}

void        WorkflowRenderer::previousFrame()
{

}

void        WorkflowRenderer::pauseMainWorkflow()
{
    qDebug() << "In pause callback";
    if ( m_paused == true )
        return ;
    m_mainWorkflow->pause();
}

void        WorkflowRenderer::mainWorkflowPaused()
{
    qDebug() << "MAIN WORKFLOW PAUSED<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<";
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
            m_paused = false;
        }
        else
        {
            //VLC will toggle play if we call pause while already paused...
            //So be careful about pausing two times :
            if ( m_paused == false )
            {
//                m_mediaPlayer->pause();
//                qDebug() << "Waiting for paused media player";
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
    m_mediaPlayer->stop();
    m_mainWorkflow->stop();
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
}

void        WorkflowRenderer::__videoStopped()
{
    emit endReached();
}
