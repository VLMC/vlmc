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
    m_buffer = new AudioSample;
    m_buffer->buff = NULL;
}

AudioClipWorkflow::~AudioClipWorkflow()
{
    if ( m_buffer->buff != NULL )
        delete[] m_buffer->buff;
    delete m_buffer;
}

void*       AudioClipWorkflow::getLockCallback()
{
    return reinterpret_cast<void*>(&AudioClipWorkflow::lock);
}

void*       AudioClipWorkflow::getUnlockCallback()
{
    return reinterpret_cast<void*>(&AudioClipWorkflow::unlock);
}

void*       AudioClipWorkflow::getOutput()
{
    QMutexLocker    lock( m_renderLock );

    if ( isEndReached() == true )
    {
        qDebug() << "Audio end reached";
        return NULL;
    }
    return m_buffer;
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
    if ( m_fullSpeedRender == true )
    {
        m_vlcMedia->addOption( ":no-sout-smem-time-sync" );
    }
    else
        m_vlcMedia->addOption( ":sout-smem-time-sync" );
}

void        AudioClipWorkflow::lock( AudioClipWorkflow* cw, uint8_t** pcm_buffer , unsigned int size )
{
    if ( cw->m_buffer->buff == NULL )
    {
        cw->m_buffer->buff = new unsigned char[size];
        cw->m_buffer->size = size;
    }
    cw->m_renderLock->lock();
    *pcm_buffer = cw->m_buffer->buff;
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
    static qint64 previous_pts = pts;
    static qint64 current_pts = pts;
    //Q_UNUSED( pts );
    previous_pts = current_pts;
    current_pts = pts;

    if ( cw->m_buffer->buff != NULL )
    {
        cw->m_buffer->nbSample = nb_samples;
        cw->m_buffer->nbChannels = channels;
        cw->m_buffer->ptsDiff = current_pts - previous_pts;
    }

    current_pts = qMax( current_pts, previous_pts );
    cw->m_renderLock->unlock();
    cw->m_stateLock->lockForWrite();

    if ( cw->m_state == Rendering )
    {
        QMutexLocker    lock( cw->m_condMutex );

        cw->m_state = Sleeping;
        cw->m_stateLock->unlock();

        {
            QMutexLocker    lock2( cw->m_renderWaitCond->getMutex() );
            cw->m_renderWaitCond->wake();
        }
        cw->emit renderComplete( cw );
//        qDebug() << "Emmiting render completed";

//        qDebug() << "Entering cond wait";
        cw->m_waitCond->wait( cw->m_condMutex );
//        qDebug() << "Leaving condwait";
        cw->m_stateLock->lockForWrite();
        if ( cw->m_state == Sleeping )
            cw->m_state = Rendering;
        cw->m_stateLock->unlock();
    }
    else
        cw->m_stateLock->unlock();
//    qDebug() << '[' << (void*)cw << "] ClipWorkflow::unlock";
    cw->checkStateChange();
}
