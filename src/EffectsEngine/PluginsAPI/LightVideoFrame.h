/*****************************************************************************
 * LightVideoFrame.h: class used to contains raw video frame, with
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

#ifndef LIGHTVIDEOFRAME_H_
#define LIGHTVIDEOFRAME_H_

#include <QSharedDataPointer>

struct	Pixel
{
  enum
    {
      NbComposantes = 3
    };
  quint8	Red;
  quint8	Green;
  quint8	Blue;
};

union	RawVideoFrame
{
  Pixel*	pixels;
  quint8*	octets;
};

struct	VideoFrame : public QSharedData
{
  ~VideoFrame();
  VideoFrame( void );
  VideoFrame( const VideoFrame& tocopy);

  RawVideoFrame	frame;
  quint32	nbpixels;
  quint32	nboctets;
};

class	LightVideoFrame
{
public:

  LightVideoFrame();
  LightVideoFrame( const LightVideoFrame& tocopy );
  LightVideoFrame( quint32 nboctets );
  LightVideoFrame( const quint8* tocopy, quint32 nboctets );
  ~LightVideoFrame();

  LightVideoFrame&	operator=( const LightVideoFrame& tocopy );
  const VideoFrame*     operator->( void ) const;
  const VideoFrame&     operator*( void ) const;
  VideoFrame*           operator->( void );
  VideoFrame&           operator*( void );

private:

  QSharedDataPointer<VideoFrame>	m_videoFrame;
};

#endif // VIDEOFRAME_H_
