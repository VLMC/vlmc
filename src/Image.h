/*****************************************************************************
 * Image.h: Image (light) managment class. (Handle the pixel buffer of a QImage)
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

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <QImage>

class   Image
{
public:
    Image( int width, int height )
    {
        m_pixelsData = new uchar[ width * height * 4 ];
        m_image = new QImage( m_pixelsData, width, height, width * 4, QImage::Format_ARGB32 );
    }
    ~Image()
    {
        delete m_image;
        delete m_pixelsData;
    }
    uchar*      getBuffer()
    {
        return m_pixelsData;
    }

    QImage&     getImage()
    {
        return *(m_image);
    }

private:
    QImage*     m_image;
    uchar*      m_pixelsData;
};

#endif // IMAGE_HPP
