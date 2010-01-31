/*****************************************************************************
 * GenericRenderer.cpp: Describe a common behavior for every renderers
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
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

#include "GenericRenderer.h"
#include "VLCMediaPlayer.h"

GenericRenderer::GenericRenderer() :
        m_paused( false ),
        m_isRendering( false )
{
    m_mediaPlayer = new LibVLCpp::MediaPlayer();
}

GenericRenderer::~GenericRenderer()
{
    delete m_mediaPlayer;
}

void
GenericRenderer::setRenderWidget(QWidget *renderWidget)
{
    m_mediaPlayer->setDrawable( renderWidget->winId() );
    m_renderWidget = renderWidget;
}
