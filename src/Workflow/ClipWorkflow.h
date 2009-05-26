/*****************************************************************************
 * ClipWorkflow.h : Clip workflow. Will extract a single frame from a VLCMedia
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

#ifndef CLIPWORKFLOW_H
#define CLIPWORKFLOW_H

//TODO: THIS HAS TO GO REALLY REALLY REALLY SOON !!!!
//FIXME:
//UGLY:
#define VIDEOWIDTH 640
#define VIDEOHEIGHT 480

#include <QReadWriteLock>
#include <QMutex>
#include <QWaitCondition>
#include <QObject>

#include "Clip.h"
#include "VLCMediaPlayer.h"

class   ClipWorkflow : public QObject
{
    Q_OBJECT

    public:
        ClipWorkflow( Clip* clip, QMutex* condMutex, QWaitCondition* waitCond );
        virtual ~ClipWorkflow();

        bool                    renderComplete() const;
        unsigned char*          getOutput();
        void                    initialize( LibVLCpp::MediaPlayer* mediaPlayer );
        bool                    isReady() const;
        bool                    isEndReached() const;
        void                    startRender();

    private:
        static void             lock( ClipWorkflow* clipWorkflow, void** pp_ret );
        static void             unlock( ClipWorkflow* clipWorkflow );
        void                    setVmem();
        void                    setRenderComplete();

    private:
        Clip*                   m_clip;
        QReadWriteLock*         m_mutex;
        bool                    m_renderComplete;
        unsigned char*          m_buffer;
        QMutex*                 m_condMutex;
        QWaitCondition*         m_waitCond;
        LibVLCpp::MediaPlayer*  m_mediaPlayer;
        QReadWriteLock*         m_initMutex;
        bool                    m_isReady;
        bool                    m_endReached;
        QReadWriteLock*         m_endReachedLock;

    public slots:
        void                    pauseAfterPlaybackStarted();
        void                    pausedMediaPlayer();
        void                    setPosition();
        void                    endReached();
};

#endif // CLIPWORKFLOW_H
