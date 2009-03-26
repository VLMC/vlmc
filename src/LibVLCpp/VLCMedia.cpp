/*****************************************************************************
 * VLCMedia.cpp: Binding for libvlc_media
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
#include <cassert>
#include "VLCMedia.h"

using namespace LibVLCpp;

Media::Media(Instance* instance, const QString& filename) : _instance(*instance), _pixelBuffer(NULL)
{
    this->_internalPtr = libvlc_media_new(this->_instance, filename.toLocal8Bit(), this->_ex);
    this->_ex.checkThrow();
}

Media::~Media()
{
    libvlc_media_release(this->_internalPtr);
    delete[] this->_pixelBuffer;
    delete this->_dataCtx;
}

Media::DataCtx*         Media::buildDataCtx()
{
    Media::DataCtx* dataCtx = new Media::DataCtx;
    dataCtx->mutex = new QMutex();
    dataCtx->media = this;
    return dataCtx;
}

void                    Media::addOption(const char* opt)
{
    libvlc_media_add_option(this->_internalPtr, opt, this->_ex);
    this->_ex.checkThrow();
    qDebug() << "Added media option: " << opt;
}

Media::DataCtx::~DataCtx()
{
    delete this->mutex;
}

void                    Media::setLockCallback(Media::lockCallback callback)
{
    char    param[64];
    sprintf(param, ":vmem-lock=%lld", (long long int)(intptr_t)callback);
    this->addOption(param);
}

void                    Media::setUnlockCallback(Media::unlockCallback callback)
{
    char    param[64];
    sprintf(param, ":vmem-unlock=%lld", (long long int)(intptr_t)callback);
    this->addOption(param);
}

void                    Media::setDataCtx()
{
    char    param[64];

    this->_dataCtx = new Media::DataCtx;
    this->_dataCtx->mutex = new QMutex();
    this->_dataCtx->media = this;

    sprintf(param, ":vmem-data=%lld", (long long int)(intptr_t)this->_dataCtx);
    this->addOption(param);
}

void                    Media::outputInVmem()
{
    this->addOption(":vout=vmem");
}

void                    Media::outputInWindow()
{
//    this->addOption();
}

void                    Media::setPixelBuffer(uchar* buffer)
{
    this->_pixelBuffer = buffer;
}

uchar*                  Media::getPixelBuffer()
{
    return this->_pixelBuffer;
}
