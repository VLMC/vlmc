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

AudioClipWorkflow::AudioClipWorkflow( Clip* clip ) :
        ClipWorkflow( clip )
{
    for ( unsigned int i = 0; i < AudioClipWorkflow::nbBuffers; ++i )
    {
        AudioSample* as = new AudioSample;
        as->buff = NULL;
        m_availableBuffers.push_back( as );
        as->debugId = i;
    }
    debugType = 1;
}

AudioClipWorkflow::~AudioClipWorkflow()
{
    while ( m_availableBuffers.isEmpty() == false )
        delete m_availableBuffers.dequeue();
    while ( m_computedBuffers.isEmpty() == false )
        delete m_computedBuffers.dequeue();
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
//    qDebug() << "entering audio get output";
    QMutexLocker    lock( m_renderLock );
//    qDebug() << "got audio render lock";
    QMutexLocker    lock2( m_computedBuffersMutex );
//    qDebug() << "got computed buffers mutex";

    if ( preGetOutput() == false )
    {
//        qDebug() << "audio preGetOutput() returned false";
        return NULL;
    }

//    qWarning() << "Audio. Available:" << m_availableBuffers.count() << "Computed:" << m_computedBuffers.count();
    if ( isEndReached() == true )
    {
//        qDebug() << "audio end is reached";
        return NULL;
    }
    if ( mode == ClipWorkflow::Get )
        qCritical() << "A sound buffer should never be asked with 'Get' mode";
    ::StackedBuffer<AudioSample*>* buff = new StackedBuffer(
            m_computedBuffers.dequeue(), this, true );
//    qDebug() << "calling audio postGetOutput();";
    postGetOutput();
//    qDebug() << "returning audio buffer";
    return buff;
}

void        AudioClipWorkflow::initVlcOutput()
{
//    m_vlcMedia->addOption( ":verbose 3" );
//    m_vlcMedia->addOption( ":vvv" );
    m_vlcMedia->addOption( ":no-sout-video" );
    m_vlcMedia->addOption( ":no-video" );
    m_vlcMedia->addOption( ":sout=#transcode{}:smem" );
    m_vlcMedia->setAudioDataCtx( this );
    m_vlcMedia->setAudioLockCallback( reinterpret_cast<void*>( getLockCallback() ) );
    m_vlcMedia->setAudioUnlockCallback( reinterpret_cast<void*>( getUnlockCallback() ) );
    m_vlcMedia->addOption( ":sout-transcode-acodec=s16l" );
    m_vlcMedia->addOption( ":sout-transcode-samplerate=48000" );
    m_vlcMedia->addOption( ":sout-transcode-channels=2" );
    m_vlcMedia->addOption( ":sout-smem-time-sync" );
//    m_vlcMedia->addOption( ":no-sout-smem-time-sync" );
}

AudioClipWorkflow::AudioSample*    AudioClipWorkflow::createBuffer( size_t size )
{
    qDebug() << "Creating new buffer of size:" << size;
    AudioSample* as = new AudioSample;
    as->buff = new uchar[size];
    as->size = size;
    as->debugId = -1;
    return as;
}

void        AudioClipWorkflow::lock( AudioClipWorkflow* cw, uint8_t** pcm_buffer , unsigned int size )
{
    QMutexLocker    lock( cw->m_availableBuffersMutex );
    cw->m_renderLock->lock();
    cw->m_computedBuffersMutex->lock();

//    qWarning() << ">>>AudioGeneration. Available:" << cw->m_availableBuffers.count() << "Computed:" << cw->m_computedBuffers.count() << "position" << cw->m_mediaPlayer->getPosition();
    AudioSample* as = NULL;
    if ( cw->m_availableBuffers.isEmpty() == true )
    {
//        qCritical() << cw << "Late buffer generation. Spawning new audio buffer.";
        as = cw->createBuffer( size );
    }
    else
    {
        as = cw->m_availableBuffers.dequeue();
        if ( as->buff == NULL )
        {
            as->buff = new uchar[size];
            as->size = size;
        }
    }
    cw->m_computedBuffers.enqueue( as );
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
    AudioSample* as = cw->m_computedBuffers.last();
    if ( as->buff != NULL )
    {
        as->nbSample = nb_samples;
        as->nbChannels = channels;
        as->ptsDiff = cw->m_currentPts - cw->m_previousPts;
    }
//    qWarning() << "::::Computing audio PTS: debugId:" << as->debugId << "ptsdiff:" << as->ptsDiff;
    cw->commonUnlock();
    cw->m_renderLock->unlock();
    cw->m_computedBuffersMutex->unlock();
}

uint32_t    AudioClipWorkflow::getNbComputedBuffers() const
{
    return m_computedBuffers.count();
}

uint32_t    AudioClipWorkflow::getMaxComputedBuffers() const
{
    return AudioClipWorkflow::nbBuffers;
}

void        AudioClipWorkflow::releaseBuffer( AudioSample *sample )
{
    QMutexLocker    lock( m_availableBuffersMutex );
    m_availableBuffers.enqueue( sample );
}

void        AudioClipWorkflow::flushComputedBuffers()
{
    QMutexLocker    lock( m_availableBuffersMutex );
    QMutexLocker    lock2( m_computedBuffersMutex );

    while ( m_computedBuffers.isEmpty() == false )
    {
        m_availableBuffers.enqueue( m_computedBuffers.dequeue() );
    }
}

AudioClipWorkflow::StackedBuffer::StackedBuffer( AudioClipWorkflow::AudioSample *as,
                                                        AudioClipWorkflow *poolHandler,
                                                        bool mustBeReleased) :
    ::StackedBuffer<AudioClipWorkflow::AudioSample*>( as, mustBeReleased ),
    m_poolHandler( poolHandler )
{
}

void    AudioClipWorkflow::StackedBuffer::release()
{
    if ( m_mustRelease == true && m_poolHandler.isNull() == false )
        m_poolHandler->releaseBuffer( m_buff );
    delete this;
}
