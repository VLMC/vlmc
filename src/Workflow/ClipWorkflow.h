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
        enum        State
        {
            Stopped,
            Initializing,
            Ready,
            Rendering,
            EndReached,
            StopScheduled,
        };

        ClipWorkflow( Clip* clip, QMutex* renderMutex, QMutex* condMutex, QWaitCondition* waitCond );
        virtual ~ClipWorkflow();

        /**
         *  This method returns the current frame. It locks the renderMutex,
         *  therefore, you can call this method blindly, without taking care
         *  of the rendering process advancement.
         */
        unsigned char*          getOutput();
        void                    initialize( LibVLCpp::MediaPlayer* mediaPlayer );
        /**
         *  Return true ONLY if the state is equal to Ready.
         *  If the state is Rendering, EndReached or anything else, this will
         *  return false.
         */
        bool                    isReady() const;
        /**
         *  Return true ONLY if the state is equal to EndReached.
         *  In any other cases, this will return false.
         */
        bool                    isEndReached() const;

        /**
         *  Return true ONLY if the state is equal to Stopped.
         *  In any other cases, this will return false.
         */
        bool                    isStopped() const;

        /**
         *  Return true ONLY if the state is equal to Stopped.
         *  In any other cases, this will return false.
         */
        bool                    isRendering() const;

        /**
         *  Returns the current workflow state.
         */
        State                   getState() const;

        /**
         *  This method start the effective render, ie calling the play() method
         *  on the media player.
         *  If the media player isn't ready, this method waits.
         */
        void                    startRender();

        /**
            \brief              Returns the Clip this workflow instance is based
                                uppon, so that you can query information on it.
            \return             A pointer to a constant clip instance.
        */
        const Clip*             getClip() const;
        /**
            \brief  Stop this workflow.
        */
        void                    stop();
        void                    setPosition( float pos );

        void                    scheduleStop();

    private:
        static void             lock( ClipWorkflow* clipWorkflow, void** pp_ret );
        static void             unlock( ClipWorkflow* clipWorkflow );
        void                    setVmem();
        void                    setState( State state );

    private:
        Clip*                   m_clip;
        unsigned char*          m_buffer;

        QMutex*                 m_renderMutex;
        QMutex*                 m_condMutex;
        QWaitCondition*         m_waitCond;

        LibVLCpp::MediaPlayer*  m_mediaPlayer;

        State                   m_state;
        QReadWriteLock*         m_stateLock;


    public slots:
        void                    pauseAfterPlaybackStarted();
        void                    pausedMediaPlayer();
        void                    setPositionAfterPlayback();
        void                    endReached();
};

#endif // CLIPWORKFLOW_H
