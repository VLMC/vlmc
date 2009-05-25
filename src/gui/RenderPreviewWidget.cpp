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
}


RenderPreviewWidget::~RenderPreviewWidget()
{
    delete m_media;
}

void*   RenderPreviewWidget::lock( void* datas )
{
//    qDebug() << "Locking invmem";
    RenderPreviewWidget* self = reinterpret_cast<RenderPreviewWidget*>( datas);
    return self->m_mainWorkflow->getOutput();
}

void    RenderPreviewWidget::unlock( void*  )
{
//    qDebug() << "Unlocking invmem";
}

void        RenderPreviewWidget::startPreview( Media* )
{
    qDebug() << "Starting render preview";
    m_mainWorkflow->startRender();
    m_mediaPlayer->play();
}

void        RenderPreviewWidget::setPosition( float /*newPos*/ )
{
}

void        RenderPreviewWidget::togglePlayPause( bool /*forcePause*/ )
{
    startPreview( NULL );
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
    qDebug() << "RenderPreviewWidget::__positionChanged: Unimplemented";
}

void        RenderPreviewWidget::__videoPaused()
{
    qDebug() << "RenderPreviewWidget::__videoPaused: Unimplemented";
}

void        RenderPreviewWidget::__videoPlaying()
{
    qDebug() << "RenderPreviewWidget::__videoPlaying: Unimplemented";
}
