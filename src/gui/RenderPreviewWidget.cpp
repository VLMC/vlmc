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

#include "RenderPreviewWidget.h"

RenderPreviewWidget::RenderPreviewWidget( MainWorkflow* mainWorkflow, QWidget* renderWidget ) :
            GenericPreviewWidget( renderWidget ), m_mainWorkflow( mainWorkflow )
{
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

    connect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( __videoPlaying() ) );
    connect( m_mediaPlayer, SIGNAL( paused() ), this, SLOT( __videoPaused() ) );
    connect( m_mainWorkflow, SIGNAL( positionChanged( float ) ), this, SLOT( __positionChanged( float ) ) );
}


RenderPreviewWidget::~RenderPreviewWidget()
{
    delete m_media;
}

void*   RenderPreviewWidget::lock( void* datas )
{
//    qDebug() << "Locking invmem";
    RenderPreviewWidget* self = reinterpret_cast<RenderPreviewWidget*>( datas);
    void* ret = self->m_mainWorkflow->getOutput();
    if ( ret == NULL )
    {
        //maybe we should display a black screen here to really emphasize
        //the end of the render
        self->m_mediaPlayer->stop();
    }
    return ret;
}

void    RenderPreviewWidget::unlock( void*  )
{
//    qDebug() << "Unlocking invmem";
}

void        RenderPreviewWidget::startPreview( Media* )
{
    m_mainWorkflow->startRender();
    m_mediaPlayer->play();
    m_isRendering = true;
}

void        RenderPreviewWidget::setPosition( float newPos )
{
    m_mainWorkflow->setPosition( newPos );
}

void        RenderPreviewWidget::togglePlayPause( bool /*forcePause*/ )
{
    if ( m_isRendering == false )
        startPreview( NULL );
    else
    {
        if ( m_mediaPlayer->isPlaying() == false )
            m_mediaPlayer->play();
        else
            m_mediaPlayer->pause();
    }
}

/////////////////////////////////////////////////////////////////////
/////SLOTS :
/////////////////////////////////////////////////////////////////////

void        RenderPreviewWidget::__endReached()
{
    qDebug() << "RenderPreviewWidget::__endReached : unimplemented";
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
