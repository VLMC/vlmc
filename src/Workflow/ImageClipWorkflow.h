/*****************************************************************************
 * ImageClipWorkflow.h : Will extract a frame from an image
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

#ifndef IMAGECLIPWORKFLOW_H
#define IMAGECLIPWORKFLOW_H

#include "ClipWorkflow.h"
#include "StackedBuffer.hpp"

class   ImageClipWorkflow : public ClipWorkflow
{
    Q_OBJECT

    public:
        class   StackedBuffer : public ::StackedBuffer<LightVideoFrame*>
        {
            public:
                StackedBuffer( LightVideoFrame* lvf );
                virtual void    release();
        };
        ImageClipWorkflow( Clip* clip );

        void                    *getLockCallback() const;
        void                    *getUnlockCallback() const;
        virtual void            *getOutput( ClipWorkflow::GetMode mode );
    protected:
        virtual void            initVlcOutput();
        virtual quint32         getNbComputedBuffers() const;
        virtual quint32         getMaxComputedBuffers() const;
        virtual void            flushComputedBuffers();
    private:
        static void             lock( ImageClipWorkflow* clipWorkflow, void** pp_ret,
                                      int size );
        static void             unlock( ImageClipWorkflow* clipWorkflow, void* buffer,
                                        int width, int height, int bpp, int size,
                                        qint64 pts );
    private:
        LightVideoFrame         *m_buffer;
        StackedBuffer           *m_stackedBuffer;

    private slots:
        void                    stopComputation();
    signals:
        void                    computedFinished();
};

#endif // IMAGECLIPWORKFLOW_H
