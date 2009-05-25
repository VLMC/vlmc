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
}

RenderPreviewWidget::~RenderPreviewWidget()
{
}

void        RenderPreviewWidget::startPreview( Media* )
{
    qDebug() << "Starting render preview";
}

void        RenderPreviewWidget::setPosition( float /*newPos*/ )
{
}

void        RenderPreviewWidget::togglePlayPause( bool /*forcePause*/ )
{

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
