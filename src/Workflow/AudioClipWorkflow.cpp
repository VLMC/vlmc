/*****************************************************************************
 * AudioClipWorkflow.cpp : Clip workflow. Will extract a single frame from a VLCMedia
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

#include "AudioClipWorkflow.h"
#include "StackedBuffer.hpp"

AudioClipWorkflow::AudioClipWorkflow( Clip* clip ) :
        ClipWorkflow( clip )
{
    m_availableBuffersLock = new QReadWriteLock;
    m_computedBuffersLock = new QReadWriteLock;
}

AudioClipWorkflow::~AudioClipWorkflow()
{
    while ( m_availableBuffers.isEmpty() == false )
        delete m_availableBuffers.pop();
    while ( m_computedBuffers.isEmpty() == false )
        delete m_computedBuffers.pop();
    delete m_availableBuffersLock;
    delete m_computedBuffersLock;
}

void*       AudioClipWorkflow::getLockCallback()
{
    return reinterpret_cast<void*>(&AudioClipWorkflow::lock);
}

void*       AudioClipWorkflow::getUnlockCallback()
{
    return reinterpret_cast<void*>(&AudioClipWorkflow::unlock);
}

void*       AudioClipWorkflow::getOutput( ClipWorkflow::GetMode mode )
{
    QMutexLocker    lock( m_renderLock );

    preGetOutput();
    if ( isEndReached() == true )
    {
        qDebug() << "Audio end reached";
        return NULL;
    }
    if ( mode == ClipWorkflow::Get )
        qCritical() << "A sound buffer should never be asked with 'Get' mode";
    StackedBuffer<AudioSample*>* buff = new StackedBuffer<AudioSample*>( m_computedBuffers.pop(), &m_availableBuffers, true );
    postGetOutput();
    return buff;
}

void        AudioClipWorkflow::initVlcOutput()
{
    m_vlcMedia->addOption( ":no-sout-video" );
    m_vlcMedia->addOption( ":sout=#transcode{}:smem" );
    m_vlcMedia->setAudioDataCtx( this );
    m_vlcMedia->setAudioLockCallback( reinterpret_cast<void*>( getLockCallback() ) );
    m_vlcMedia->setAudioUnlockCallback( reinterpret_cast<void*>( getUnlockCallback() ) );
    m_vlcMedia->addOption( ":sout-transcode-acodec=s16l" );
    m_vlcMedia->addOption( ":sout-transcode-samplerate=48000" );
    m_vlcMedia->addOption( ":sout-transcode-channels=2" );
    m_vlcMedia->addOption( ":no-sout-smem-time-sync" );
}

void        AudioClipWorkflow::lock( AudioClipWorkflow* cw, uint8_t** pcm_buffer , unsigned int size )
{
    //If there's no buffer at all, it must be the first render
    if ( cw->m_availableBuffers.count() == 0 && cw->m_computedBuffers.count() == 0 )
    {
        for ( unsigned int i = 0; i < AudioClipWorkflow::nbBuffers; ++i )
        {
            AudioSample* as = new AudioSample;
            as->buff = new uchar[size];
            as->size = size;
            cw->m_availableBuffers.push_back( as );
        }
    }
    cw->m_renderLock->lock();
    AudioSample* as = cw->m_availableBuffers.pop();
    cw->m_computedBuffers.push_back( as );
    *pcm_buffer = as->buff;
}

void        AudioClipWorkflow::unlock( AudioClipWorkflow* cw, uint8_t* pcm_buffer,
                                      unsigned int channels, unsigned int rate,
                                      unsigned int nb_samples, unsigned int bits_per_sample,
                                      unsigned int size, qint64 pts )
{
//    qDebug() << "pts:" << pts << "nb channels" << channels << "rate:" << rate <<
//            "size:" << size << "nb_samples:" << nb_samples;
    Q_UNUSED( pcm_buffer );
    Q_UNUSED( rate );
    Q_UNUSED( bits_per_sample );
    Q_UNUSED( size );

    cw->computePtsDiff( pts );
    AudioSample* as = cw->m_computedBuffers.head();
    if ( as->buff != NULL )
    {
        as->nbSample = nb_samples;
        as->nbChannels = channels;
        as->ptsDiff = cw->m_currentPts - cw->m_previousPts;
    }
    cw->m_renderLock->unlock();

    cw->commonUnlock();
}

uint32_t    AudioClipWorkflow::getAvailableBuffers() const
{
    return m_availableBuffers.count();
}

uint32_t    AudioClipWorkflow::getComputedBuffers() const
{
    return m_computedBuffers.count();
}
