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
#include "WaitCondition.hpp"
#include <QObject>

#include "Clip.h"
#include "VLCMediaPlayer.h"

class   ClipWorkflow : public QObject
{
    Q_OBJECT

    public:
        enum        State
        {
            None = -1,
            Stopped,        //0
            Initializing,   //1
            Ready,          //2
            Rendering,      //3
            Sleeping,       //4
            Pausing,        //5
            Paused,         //6
            ThreadPaused,   //7
            Stopping,       //8
            EndReached,     //9
        };
       int                     debugId;

        ClipWorkflow( Clip* clip );
        virtual ~ClipWorkflow();

        /**
         *  This method returns the current frame. It locks the renderMutex,
         *  therefore, you can call this method blindly, without taking care
         *  of the rendering process advancement.
         */
        unsigned char*          getOutput();
        void                    initialize();
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
         *  Return true ONLY if the state is equal to Rendering.
         *  In any other cases, this will return false.
         */
        bool                    isRendering() const;

        bool                    isPausing() const;

        bool                    isThreadPaused() const;

        /**
         *  Returns the current workflow state.
         *  Be carrefull, as this function is NOT thread safe, and return the
         *  state without locking the state.
         *  It's your job to do it, by calling the getStateLock() method.
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
        Clip*                   getClip();
        /**
            \brief  Stop this workflow.
        */
        void                    stop();
        void                    pause();
        void                    setPosition( float pos );

        /**
         *  This method must be used to change the state of the ClipWorkflow
         *  from outside its render loop, otherwise, it may lead to deadlocks.
         *  No additional operations will be executed (for example, if setting
         *  the new state to stop, the media player won't be stopped.)
         *  This is mainly to change the behaviour of the render loop.
         */
        void                    queryStateChange( State newState );

        /**
         *  This method will wake the renderer thread for one iteration.
         */
        void                    wake();

        /**
         *  This returns the QReadWriteLock that protects the ClipWorkflow's state.
         *  It should be use to lock the value when checking states from outside this
         *  class.
         */
        QReadWriteLock*         getStateLock();

        /**
         *  Put back the ClipWorkflow in its initial state.
         */
        void                    reinitialize();

        void                    unpause( bool wakeRenderThread = true );

        void                    waitForCompleteInit();
        void                    waitForCompleteRender( bool dontCheckRenderStarted = false );
        void                    waitForPausingState();
        void                    waitForPausedThread();
        QMutex*                 getSleepMutex();

        LibVLCpp::MediaPlayer*  getMediaPlayer();

//        void                    activateOneFrameOnly();

    private:
        static void             lock( ClipWorkflow* clipWorkflow, void** pp_ret );
        static void             unlock( ClipWorkflow* clipWorkflow );
        void                    setVmem();
        void                    setState( State state );
        void                    checkSynchronisation( State newState );
        /**
         *  Don't ever call this method from anywhere else than the unlock() method
         */
        void                    checkStateChange();

    private:
        Clip*                   m_clip;

        /**
         *  \brief  The VLC media used to render
         */
        LibVLCpp::Media*        m_vlcMedia;

        unsigned char*          m_buffer;
        //unsigned char*          m_backBuffer;
        /**
         *  This allow the render procedure to know in which buffer it should render.
         *  If true, then the render occurs in the back buffer, which means the
         *  returned buffer much be the "front" buffer.
         *  In other term :
         *  - When m_usingBackBuffer == false, lock() will return m_buffer, and getOutput() m_backBuffer
         *  - When m_usingBackBuffer == true, lock() will return m_backBuffer, and getOutput() m_buffer
         */
        //bool                    m_usingBackBuffer;
        //QReadWriteLock*         m_backBufferLock;

        LibVLCpp::MediaPlayer*  m_mediaPlayer;

        QMutex*                 m_condMutex;
        QWaitCondition*         m_waitCond;

        State                   m_state;
        QReadWriteLock*         m_stateLock;
        State                   m_requiredState;
        QMutex*                 m_requiredStateLock;

        QAtomicInt              m_oneFrameOnly;

        WaitCondition*          m_initWaitCond;
        WaitCondition*          m_renderWaitCond;
        WaitCondition*          m_pausingStateWaitCond;
        WaitCondition*          m_pausedThreadCondWait;

    private slots:
        void                    pauseAfterPlaybackStarted();
        void                    initializedMediaPlayer();
        void                    setPositionAfterPlayback();

    public slots:
        void                    clipEndReached();

    signals:
        void                    renderComplete( ClipWorkflow* );
};

#endif // CLIPWORKFLOW_H
