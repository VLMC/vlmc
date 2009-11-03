/*****************************************************************************
 * WorkflowRenderer.h: Render the main workflow
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

#ifndef WORKFLOWRENDERER_H
#define WORKFLOWRENDERER_H


#include <QObject>
#include <QWidget>
#include <QStack>

#include "Workflow/MainWorkflow.h"
#include "GenericRenderer.h"

class   WorkflowRenderer : public GenericRenderer
{
    Q_OBJECT
    Q_DISABLE_COPY( WorkflowRenderer )

    public:
        enum    Actions
        {
            Pause,
            //Unpause,
        };
        enum    EsType
        {
            Unknown,
            Audio,
            Video,
            Subtitle //This is clearly not used by VLMC, but it fits imem module's model
        };
        struct  EsHandler
        {
            WorkflowRenderer*   self;
            EsType              type;
        };
        WorkflowRenderer();
        ~WorkflowRenderer();

        /**
            \brief          Set the preview position
            \param          newPos : The new position in vlc position (between
                                        0 and 1)
        */
        virtual void        setPosition( float newPos );
        virtual void        togglePlayPause( bool forcePause );
        virtual void        stop();
        virtual void        nextFrame();
        virtual void        previousFrame();
        virtual qint64      length() { return 0; }

        static int          lock( void *data, int64_t *dts, int64_t *pts, unsigned int *flags, size_t *bufferSize, void **buffer );
        static int          lockVideo( WorkflowRenderer* self, int64_t *pts, size_t *bufferSize, void **buffer );
        static int          lockAudio( WorkflowRenderer* self, int64_t *pts, size_t *bufferSize, void **buffer );
        static void         unlock( void *data, size_t buffSize, void *buffer );

    private:
        void                internalPlayPause( bool forcePause );
        void                pauseMainWorkflow();
        void                unpauseMainWorkflow();
        virtual void        startPreview();
        void                checkActions();

    protected:
        MainWorkflow*       m_mainWorkflow;
        LibVLCpp::Media*    m_media;
        bool                m_stopping;

    private:
        unsigned char*	    m_renderVideoFrame;
        size_t              m_videoBuffSize;
        AudioClipWorkflow::AudioSample* m_renderAudioSample;
        QStack<Actions>     m_actions;
        QReadWriteLock*     m_actionsLock;
        bool                m_pauseAsked;
        bool                m_unpauseAsked;
        QMutex*             m_condMutex;
        QWaitCondition*     m_waitCond;
        EsHandler*          m_videoEsHandler;
        EsHandler*          m_audioEsHandler;
        /**
         *  \brief          This isn't exactly the current PTS.
         *                  It's the number of frame rendered since the render has started.
         */
        qint64              m_pts;
        qint64              m_audioPts;

    public slots:
        virtual void        setClip( Clip* ){}
        virtual void        setMedia( Media* ) {}
        void                mediaUnloaded( const QUuid& ) {}
        void                timelineCursorChanged( qint64 newFrame );

        void                __positionChanged();
        void                __positionChanged( float pos );
        void                __videoPaused();
        void                __videoStopped();
        void                __videoPlaying();
        void                __endReached();

    private slots:
        void                mainWorkflowPaused();
        void                mainWorkflowUnpaused();

};

#endif // WORKFLOWRENDERER_H
