/*****************************************************************************
 * AudioClipWorkflow.h : Clip workflow. Will extract a single frame from a VLCMedia
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

#ifndef AUDIOCLIPWORKFLOW_H
#define AUDIOCLIPWORKFLOW_H

#include "ClipWorkflow.h"
#include "StackedBuffer.hpp"

class   AudioClipWorkflow : public ClipWorkflow
{
    public:
        struct  AudioSample
        {
            unsigned char*  buff;
            size_t          size;
            unsigned int    nbSample;
            unsigned int    nbChannels;
            qint64          ptsDiff;
            uint32_t        debugId;
        };
        class   StackedBuffer : public ::StackedBuffer<AudioSample*>
        {
            public:
                StackedBuffer( AudioSample* lvf, AudioClipWorkflow* poolHandler,
                                    bool mustBeReleased = true);
                virtual void        release();
            private:
                AudioClipWorkflow*  m_poolHandler;
        };

        AudioClipWorkflow( Clip* clip );
        ~AudioClipWorkflow();
        void*                   getLockCallback();
        void*                   getUnlockCallback();
        virtual void*           getOutput( ClipWorkflow::GetMode mode );

    protected:
        virtual uint32_t        getNbComputedBuffers() const;
        virtual uint32_t        getMaxComputedBuffers() const;
        void                    flushComputedBuffers();

    private:
        void                    releaseBuffer( AudioSample* sample );

    private:
        QQueue<AudioSample*>        m_computedBuffers;
        QQueue<AudioSample*>        m_availableBuffers;
        void                        initVlcOutput();
        AudioSample*                createBuffer( size_t size );
        static void                 lock( AudioClipWorkflow* clipWorkflow, uint8_t** pcm_buffer , unsigned int size );
        static void                 unlock( AudioClipWorkflow* clipWorkflow, uint8_t* pcm_buffer,
                                            unsigned int channels, unsigned int rate,
                                            unsigned int nb_samples, unsigned int bits_per_sample,
                                            unsigned int size, qint64 pts );

        //FIXME: this is totally random powered ! Please adjust with a value that does make sense...
        static const uint32_t   nbBuffers = 128;
};

#endif // AUDIOCLIPWORKFLOW_H
