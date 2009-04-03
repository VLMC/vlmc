/*****************************************************************************
 * OutputMedia.cpp: Class for outpouting a media from a VMEM
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
#include "OutputMedia.h"

OutputMedia::OutputMedia( LibVLCpp::Instance* instance ) : Media( instance,"fake://" ), m_pixelBuffer( NULL )
{
    m_dataCtx = new OutputMedia::DataCtx;
    m_dataCtx->mutex = new QMutex;
    m_dataCtx->outputMedia = this;

    char    width[64], height[64], lock[64], unlock[64], data[64];
    sprintf( width, ":invmem-width=%i", VIDEOWIDTH );
    sprintf( height, ":invmem-height=%i", VIDEOHEIGHT );
    sprintf( lock, ":invmem-lock=%lld", (qint64)(intptr_t)&OutputMedia::lock );
    sprintf( unlock, ":invmem-unlock=%lld", (qint64)(intptr_t)&OutputMedia::unlock );
    sprintf( data, ":invmem-data=%lld", (qint64)(intptr_t)m_dataCtx );

    addParam( width );
    addParam( height );
    addParam( lock );
    addParam( unlock );
    addParam( data );
    addParam( ":vout=sdl" );
}

uchar*          OutputMedia::lock( OutputMedia::DataCtx* dataCtx )
{
    qDebug() << "Copying into invmem";
    dataCtx->mutex->lock();
    return dataCtx->outputMedia->m_pixelBuffer;
}

void            OutputMedia::unlock( OutputMedia::DataCtx* dataCtx )
{
    //FIXME: use class Image to avoid alloc/free...
    //delete dataCtx->outputMedia->m_pixelBuffer;
    dataCtx->mutex->unlock();
    qDebug() << "Unlocked invmem";
}

void            OutputMedia::setVmem( uchar* pixelBuffer )
{
    m_dataCtx->mutex->lock();
    m_pixelBuffer = pixelBuffer;
    m_dataCtx->mutex->unlock();
}

void            OutputMedia::play()
{
    Media::play();
}
