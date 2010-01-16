/*****************************************************************************
 * ImageClipWorkflow.h : Will extract a frame from an image
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

#ifndef IMAGECLIPWORKFLOW_H
#define IMAGECLIPWORKFLOW_H

#include "VideoClipWorkflow.h"

class   ImageClipWorkflow : public VideoClipWorkflow
{
    Q_OBJECT

    public:
        class   StackedBuffer : public ::StackedBuffer<LightVideoFrame*>
        {
            public:
                StackedBuffer( LightVideoFrame* lvf );
                virtual void    release();
            private:
                QPointer<VideoClipWorkflow>     m_poolHandler;
        };
        ImageClipWorkflow( Clip* clip );

        void*                   getLockCallback();
        void*                   getUnlockCallback();
        virtual void            *getOutput( ClipWorkflow::GetMode mode );
    protected:
        virtual void            initVlcOutput();
        virtual uint32_t        getNbComputedBuffers() const;
        virtual uint32_t        getMaxComputedBuffers() const;
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
