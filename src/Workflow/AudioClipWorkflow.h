/*****************************************************************************
 * AudioClipWorkflow.h : Clip workflow. Will extract a single frame from a VLCMedia
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

#ifndef AUDIOCLIPWORKFLOW_H
#define AUDIOCLIPWORKFLOW_H

#include "ClipWorkflow.h"
#include "StackedBuffer.hpp"

#include <QPointer>

class   AudioClipWorkflow : public ClipWorkflow
{
    Q_OBJECT

    public:
        struct  AudioSample
        {
            unsigned char*  buff;
            size_t          size;
            quint32         nbSample;
            quint32         nbChannels;
            qint64          ptsDiff;
            quint32         debugId;
        };
        class   StackedBuffer : public ::StackedBuffer<AudioSample*>
        {
            public:
                StackedBuffer( AudioSample* lvf, AudioClipWorkflow* poolHandler,
                                    bool mustBeReleased = true);
                virtual void        release();
            private:
                QPointer<AudioClipWorkflow>     m_poolHandler;
        };

        AudioClipWorkflow( Clip* clip );
        ~AudioClipWorkflow();
        void                    *getLockCallback() const;
        void                    *getUnlockCallback() const;
        virtual void            *getOutput( ClipWorkflow::GetMode mode );

    protected:
        virtual quint32        getNbComputedBuffers() const;
        virtual quint32        getMaxComputedBuffers() const;
        void                    flushComputedBuffers();

    private:
        void                    releaseBuffer( AudioSample* sample );

    private:
        QQueue<AudioSample*>        m_computedBuffers;
        QQueue<AudioSample*>        m_availableBuffers;
        void                        initVlcOutput();
        AudioSample*                createBuffer( size_t size );
        static void                 lock( AudioClipWorkflow* clipWorkflow,
                                          quint8** pcm_buffer , quin32 size );
        static void                 unlock( AudioClipWorkflow* clipWorkflow,
                                            quint8* pcm_buffer, quint32 channels,
                                            quint32 rate, quint32 nb_samples,
                                            quint32 bits_per_sample,
                                            quint32 size, qint64 pts );

        //FIXME: this is totally random powered ! Please adjust with a value that does make sense...
        static const quint32   nbBuffers = 256;
};

#endif // AUDIOCLIPWORKFLOW_H
