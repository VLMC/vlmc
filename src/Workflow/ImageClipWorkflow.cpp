/*****************************************************************************
 * ImageClipWorkflow.cpp : Will extract a frame from an image
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

#include "ImageClipWorkflow.h"

#define IMAGE_DURATION  10000

ImageClipWorkflow::ImageClipWorkflow( Clip* clip ) : VideoClipWorkflow( clip )
{
}

void    ImageClipWorkflow::initVlcOutput()
{
    char    buffer[32];

    sprintf( buffer, ":fake-duration=%d", IMAGE_DURATION );
    m_vlcMedia->addOption( buffer );
    sprintf( buffer, ":fake-fps=%f", m_clip->getParent()->getFps() );
    m_vlcMedia->addOption( buffer );

    VideoClipWorkflow::initVlcOutput();
}
