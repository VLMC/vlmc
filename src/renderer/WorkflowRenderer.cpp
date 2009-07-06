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
            m_framePlayed( false )
{
    m_framePlayedLock = new QReadWriteLock;
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

    delete m_media;
}

void*   WorkflowRenderer::lock( void* datas )
{
    WorkflowRenderer* self = reinterpret_cast<WorkflowRenderer*>( datas );
    void* ret = self->m_mainWorkflow->getOutput();
    return ret;
}

void    WorkflowRenderer::unlock( void* datas )
{
    WorkflowRenderer* self = reinterpret_cast<WorkflowRenderer*>( datas );

    QWriteLocker    lock( self->m_framePlayedLock );
    self->m_framePlayed = true;
}

void        WorkflowRenderer::stopPreview()
{
    disconnect( m_mainWorkflow, SIGNAL( frameChanged(qint64) ),
             Timeline::getInstance()->tracksView()->tracksCursor(), SLOT( updateCursorPos( qint64 ) ) );
    stop();
}

void        WorkflowRenderer::startPreview( Media* )
{
    char        buff[128];

    connect( m_mainWorkflow, SIGNAL( frameChanged(qint64) ),
             Timeline::getInstance()->tracksView()->tracksCursor(), SLOT( updateCursorPos( qint64 ) ) );
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
    {
        QWriteLocker    lock( m_framePlayedLock );
        m_framePlayed = false;
    }
    m_mainWorkflow->nextFrame();
    m_mediaPlayer->play();
    bool    framePlayed = false;
    while ( framePlayed == false )
    {
        SleepMS( 50 );
        QReadLocker lock( m_framePlayedLock );
        framePlayed = m_framePlayed;
    }
    m_mediaPlayer->pause();
}

void        WorkflowRenderer::previousFrame()
{

}

void        WorkflowRenderer::togglePlayPause( bool forcePause )
{
    //If force pause is true, we just ensure that this render is paused... no need to start it.
    if ( m_isRendering == false && forcePause == false )
        startPreview( NULL );
    else if ( m_isRendering == true )
    {
        if ( m_paused == true && forcePause == false )
        {
            m_mediaPlayer->play();
            m_paused = false;
        }
        else
        {
            //VLC will toggle play if we call pause while already paused...
            //So be careful about pausing two times :
            if ( m_paused == false )
            {
                m_mediaPlayer->pause();
                m_paused = true;
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
    emit paused();
}

void        WorkflowRenderer::__videoPlaying()
{
    emit playing();
}

void        WorkflowRenderer::__videoStopped()
{
    emit endReached();
}
