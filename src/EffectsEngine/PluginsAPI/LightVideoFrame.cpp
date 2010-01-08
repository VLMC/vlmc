/*****************************************************************************
 * LightVideoFrame.cpp: class used to contains raw video frame, with
 * copy-on-write and references conter mecanism for performance and avoiding
 * memory leak
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Vincent Carrubba <cyberbouba@gmail.com>
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

#include "LightVideoFrame.h"

#include <QSharedData>
#include <QDebug>
#include <qmutex.h>
#include <QWriteLocker>
#include <QReadLocker>

VideoFrame::~VideoFrame()
{
  if ( frame.octets != NULL )
    delete [] frame.octets;
}

VideoFrame::VideoFrame( void )
{
  frame.octets = NULL;
  nboctets = 0;
  nbpixels = 0;
}

VideoFrame::VideoFrame( const VideoFrame& tocopy ) : QSharedData( tocopy )
{
    if ( tocopy.frame.octets != NULL )
    {
        nboctets = tocopy.nboctets;
        nbpixels = tocopy.nboctets / Pixel::NbComposantes;
        frame.octets = new quint8[tocopy.nboctets];

        memcpy( frame.octets, tocopy.frame.octets, nboctets );
    }
    else
    {
        nboctets = 0;
        nbpixels = 0;
        frame.octets = NULL;
    }
}

//
//
//
//
//

LightVideoFrame::LightVideoFrame()
{
  m_videoFrame = new VideoFrame;
}

LightVideoFrame::LightVideoFrame( const LightVideoFrame& tocopy ) : m_videoFrame(tocopy.m_videoFrame)
{
}

LightVideoFrame&
LightVideoFrame::operator=( const LightVideoFrame& tocopy )
{
  m_videoFrame = tocopy.m_videoFrame;
  return *this;
}

LightVideoFrame::LightVideoFrame( quint32 nboctets )
{
  m_videoFrame = new VideoFrame;
  m_videoFrame->nboctets = nboctets;
  m_videoFrame->nbpixels = nboctets / Pixel::NbComposantes;
  m_videoFrame->frame.octets = new quint8[nboctets];
}

LightVideoFrame::LightVideoFrame( const quint8 * tocopy, quint32 nboctets )
{
    m_videoFrame = new VideoFrame;
    m_videoFrame->nboctets = nboctets;
    m_videoFrame->nbpixels = nboctets / Pixel::NbComposantes;
    m_videoFrame->frame.octets = new quint8[nboctets];

    memcpy( m_videoFrame->frame.octets, tocopy, m_videoFrame->nboctets );
}

LightVideoFrame::~LightVideoFrame()
{
}

const VideoFrame*
LightVideoFrame::operator->( void ) const
{
  return m_videoFrame.data();
}

const VideoFrame&
LightVideoFrame::operator*( void ) const
{
  return *m_videoFrame;
}

VideoFrame*
LightVideoFrame::operator->( void )
{
  return m_videoFrame.data();
}

VideoFrame&
LightVideoFrame::operator*( void )
{
  return *m_videoFrame;
}
