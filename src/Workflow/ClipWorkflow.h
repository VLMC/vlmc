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

#include <QReadWriteLock>
#include <QMutex>
#include <QObject>
#include <QQueue>

#include "Clip.h"
#include "WaitCondition.hpp"
#include "VLCMediaPlayer.h"
#include "LightVideoFrame.h"

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
            Stopping,       //7
            EndReached,     //8
        };

        ClipWorkflow( Clip* clip );
        virtual ~ClipWorkflow();

        /**
         *  This method returns the current frame. It locks the renderMutex,
         *  therefore, you can call this method blindly, without taking care
         *  of the rendering process advancement.
         */
        virtual void*           getOutput() = 0;
        virtual void            initVlcOutput() = 0;
        void                    initialize( bool preloading = false );

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

        bool                    isSleeping() const;

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
        void                    startRender( bool startInPausedMode );

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
        void                    setTime( qint64 time );

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

        void                    unpause();

        void                    waitForCompleteInit();

        /**
         *  \brief      Return the wait condition associated to the render.
         *              It's used when waiting for a render to terminate.
         */
        WaitCondition*          getRenderCondWait();
        void                    waitForCompleteRender( bool dontCheckRenderStarted = false );
        QMutex*                 getSleepMutex();

        virtual void*           getLockCallback() = 0;
        virtual void*           getUnlockCallback() = 0;

        LibVLCpp::MediaPlayer*  getMediaPlayer();

        void                    setFullSpeedRender( bool value );

    private:
        void                    setState( State state );
        void                    checkSynchronisation( State newState );
        void                    adjustBegin();

    private:
        LibVLCpp::MediaPlayer*  m_mediaPlayer;

        State                   m_requiredState;
        QMutex*                 m_requiredStateLock;

        WaitCondition*          m_initWaitCond;
        WaitCondition*          m_pausingStateWaitCond;

        /**
         *  While this flag is set to false, we will use the same buffer, to prevent
         *  having X buffers with the same picture (when media player is paused mainly)
         */
        bool                    m_rendering;
        /**
         *  This flag is here to avoid multiple connection to the mediaPlayer* slots.
         *  It's essentially a nasty hack due to the multiples calls to lock/unlock when
         *  the render is started, and that cannot really be avoided...
         */
        bool                    m_initFlag;

    protected:
        Clip*                   m_clip;
        QMutex*                 m_renderLock;
        QReadWriteLock*         m_stateLock;
        QMutex*                 m_condMutex;
        State                   m_state;
        WaitCondition*          m_renderWaitCond;
        QWaitCondition*         m_waitCond;
        bool                    m_fullSpeedRender;
        /**
         *  \brief  The VLC media used to render
         */
        LibVLCpp::Media*        m_vlcMedia;

    protected:
        /**
         *  Don't ever call this method from anywhere else than the unlock() method
         */
        void                    checkStateChange();

    private slots:
        /**
         *  \brief  This slot is used when preloading, to pause the mediaplayer once fully loaded.
         */
        void                    pauseAfterPlaybackStarted();
        /**
         *  \brief  When preloading, this slot is used to mark that the media player has been paused again.
         */
        void                    initializedMediaPlayer();
        void                    loadingComplete();
        void                    pausedMediaPlayer();
        void                    unpausedMediaPlayer();

    public slots:
        void                    clipEndReached();

    signals:
        void                    renderComplete( ClipWorkflow* );
        void                    paused();
        void                    unpaused();
};

#endif // CLIPWORKFLOW_H
