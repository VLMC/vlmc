/*****************************************************************************
 * RenderPreviewWidget.cpp: Allow a current workflow preview
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

#include "RenderPreviewWidget.h"

RenderPreviewWidget::RenderPreviewWidget( MainWorkflow* mainWorkflow, QWidget* renderWidget ) :
            GenericPreviewWidget( renderWidget ),
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
    sprintf( buffer, ":invmem-lock=%lld", (qint64)RenderPreviewWidget::lock );
    m_media->addOption( buffer );
    sprintf( buffer, ":invmem-unlock=%lld", (qint64)RenderPreviewWidget::unlock );
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


RenderPreviewWidget::~RenderPreviewWidget()
{
    m_mediaPlayer->stop();

    disconnect( m_mediaPlayer, SIGNAL( playing() ),    this,   SLOT( __videoPlaying() ) );
    disconnect( m_mediaPlayer, SIGNAL( paused() ),     this,   SLOT( __videoPaused() ) );
    disconnect( m_mediaPlayer, SIGNAL( stopped() ),    this,   SLOT( __videoStopped() ) );
    disconnect( m_mainWorkflow, SIGNAL( mainWorkflowEndReached() ), this, SLOT( __endReached() ) );
    disconnect( m_mainWorkflow, SIGNAL( positionChanged( float ) ), this, SLOT( __positionChanged( float ) ) );

    delete m_media;
}

void*   RenderPreviewWidget::lock( void* datas )
{
    RenderPreviewWidget* self = reinterpret_cast<RenderPreviewWidget*>( datas );
    void* ret = self->m_mainWorkflow->getOutput();
    return ret;
}

void    RenderPreviewWidget::unlock( void* datas )
{
    RenderPreviewWidget* self = reinterpret_cast<RenderPreviewWidget*>( datas );

//    qDebug() << "RenderPreviewWidget::unlock() : Frame rendered";
    QWriteLocker    lock( self->m_framePlayedLock );
    self->m_framePlayed = true;
}

void        RenderPreviewWidget::stopPreview()
{
    //This might be called multiple times, but this is due to Qt message loop
    m_mediaPlayer->stop();
    m_isRendering = false;
    m_paused = false;
}

void        RenderPreviewWidget::startPreview( Media* )
{
    m_mainWorkflow->startRender();
    m_mediaPlayer->play();
    m_isRendering = true;
    m_paused = false;
}

void        RenderPreviewWidget::setPosition( float newPos )
{
    m_mainWorkflow->setPosition( newPos );
}

void        RenderPreviewWidget::nextFrame()
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
        usleep( 50 );
        QReadLocker lock( m_framePlayedLock );
        framePlayed = m_framePlayed;
    }
    m_mediaPlayer->pause();
}

void        RenderPreviewWidget::previousFrame()
{

}

void        RenderPreviewWidget::togglePlayPause( bool forcePause )
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

void        RenderPreviewWidget::stop()
{
    m_isRendering = false;
    m_mainWorkflow->stop();
    m_mediaPlayer->stop();
}

/////////////////////////////////////////////////////////////////////
/////SLOTS :
/////////////////////////////////////////////////////////////////////

void        RenderPreviewWidget::__endReached()
{
    stopPreview();
    emit endReached();
}

void        RenderPreviewWidget::__positionChanged()
{
    qFatal("This should never be used ! Get out of here !");
}

void        RenderPreviewWidget::__positionChanged( float pos )
{
    emit positionChanged( pos );
}

void        RenderPreviewWidget::__videoPaused()
{
    emit paused();
}

void        RenderPreviewWidget::__videoPlaying()
{
    emit playing();
}

void        RenderPreviewWidget::__videoStopped()
{
    emit endReached();
}
