/*****************************************************************************
 * VideoClipWorkflow.h : Clip workflow. Will extract a single frame from a VLCMedia
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

#ifndef VIDEOCLIPWORKFLOW_H
#define VIDEOCLIPWORKFLOW_H

#include "ClipWorkflow.h"
#include "StackedBuffer.hpp"
#include "Pool.hpp"

#include <QPointer>

class   Clip;

class   VideoClipWorkflow : public ClipWorkflow
{
    Q_OBJECT

    public:
        class   StackedBuffer : public ::StackedBuffer<LightVideoFrame*>
        {
            public:
                StackedBuffer( LightVideoFrame* lvf, VideoClipWorkflow* poolHandler,
                                    bool mustBeReleased = true);
                virtual void    release();
            private:
                QPointer<VideoClipWorkflow>     m_poolHandler;
        };

        VideoClipWorkflow( Clip* clip );
        ~VideoClipWorkflow();
        void                    *getLockCallback() const;
        void                    *getUnlockCallback() const;
        virtual void            *getOutput( ClipWorkflow::GetMode mode );

        static const quint32    nbBuffers = 3 * 30; //3 seconds with an average fps of 30

    protected:
        virtual void            initVlcOutput();
        virtual quint32         getNbComputedBuffers() const;
        virtual quint32         getMaxComputedBuffers() const;
        void                    releaseBuffer( LightVideoFrame* lvf );
        void                    flushComputedBuffers();

    private:
        QQueue<LightVideoFrame*>    m_computedBuffers;
        QQueue<LightVideoFrame*>    m_availableBuffers;
        static void             lock( VideoClipWorkflow* clipWorkflow, void** pp_ret,
                                      int size );
        static void             unlock( VideoClipWorkflow* clipWorkflow, void* buffer,
                                        int width, int height, int bpp, int size,
                                        qint64 pts );
};

#endif // VIDEOCLIPWORKFLOW_H
