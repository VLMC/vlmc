/*****************************************************************************
 * WorkflowRenderer.h: Render the main workflow
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

#ifndef WORKFLOWRENDERER_H
#define WORKFLOWRENDERER_H

#include "AudioClipWorkflow.h"
#include "GenericRenderer.h"
#include "MainWorkflow.h"

#include <QObject>

class   Clip;

class   QWidget;
class   QWaitCondition;
class   QMutex;
class   QUuid;

class   WorkflowRenderer : public GenericRenderer
{
    Q_OBJECT
    Q_DISABLE_COPY( WorkflowRenderer )

    public:
        /**
         *  \brief      Describes the different types of elementary streams
         *              that can be handled by the imem.
         */
        enum    EsType
        {
            Unknown, ///< Unknown type. Should never happen
            Audio, ///< Audio type
            Video, ///< Video type
            Subtitle ///< This is clearly not used by VLMC, but it fits imem module's model
        };
        /**
         *  \brief  This struct will be the type of the callback parameter
         *          in the lock / unlock callbacks
         */
        struct  EsHandler
        {
            WorkflowRenderer*   self; ///< The 'this' pointer will be passed in this field
            EsType              type; ///< The elementary stream type
        };

        WorkflowRenderer();
        ~WorkflowRenderer();

        /**
         *  \brief      Initialize the renderer.
         *
         *  This will set the imem string, instantiate EsHandlers, connect signals.
         */
        void                initializeRenderer();
        /**
         *  \brief          Play or pause the media.
         *
         *  When this method is called :
         *      - if the render has not started and forcePause is false, the render is started
         *      - if the render has not started and forcePause is true, nothing happens.
         *      - if the render has started and is not paused, the render will pause
         *      - if the render has started, and is paused, the render will unpause if
         *          forcePause is false.
         *  \param  forcePause  Will force the pause if true.
         *  \sa     stop()
         *  \warning    Do NOT call this method from a constructor as it calls some
         *              virtual methods.
         */
        virtual void        togglePlayPause( bool forcePause );
        /**
         *  \brief Stop the mainworkflow, but not the renderer.
         *
         *  In order to provide premanent feedback (ie stay in paused mode when not
         *  playing, we have to never stop the renderer.
         *  \sa togglePlayPause( bool );
         *  \sa killRenderer();
         */
        virtual void        stop();
        /**
         *  \brief Render the next frame
         *  \sa     previousFrame()
         */
        virtual void        nextFrame();
        /**
         *  \brief  Render the previous frame
         *  \sa     nextFrame();
         */
        virtual void        previousFrame();
        /**
         * \brief   Return the length in milliseconds
         * \return  The length of the underlying rendered target in milliseconds
         *  \sa     getLength()
         */
        virtual qint64      getLengthMs() const;

        virtual qint64      getLength() const;

        /**
         *  \brief  Return the current frame number
         *  \return The current frame
         */
        virtual qint64      getCurrentFrame() const;
        /**
         *  \brief Return the number of frames per second
         *  \return     The current fps
         */
        virtual float       getFps() const;

        /**
         *  \brief          Completely kill the renderer.
         *
         *  This will stop both mainworkflow and renderer thread.
         *  This should only be used when destroying the renderer.
         *  \sa             stop();
         */
        void                killRenderer();

    private:
        /**
         *  \brief          This is a subpart of the togglePlayPause( bool ) method
         *  \sa             togglePlayPause( bool );
         */
        void                internalPlayPause( bool forcePause );
        /**
         *  \brief          This is a subpart of the togglePlayPause( bool ) method
         *
         *  It starts the render by launching the media player that will query the
         *  lock and unlock callbacks, thus querying the MainWorkflow for frames and
         *  audio samples
         *  \sa             togglePlayPause( bool );
         */
        virtual void        startPreview();

    protected:
        /**
         *  \brief          Will return a pointer to the function/static method to use
         *                  as the imem lock callback.
         *  This method is provided to allow renderers to inherit from this class,
         *  without having to reimplement the initializeRenderer() method, that uses
         *  both getLockCallback() and getUnlockCallback()
         *  \return         A pointer to the lock function.
         *  \sa             getUnlockCallback()
         */
        virtual void*       getLockCallback();
        /**
         *  \brief          Will return a pointer to the function/static method to use
         *                  as the imem unlock callback.
         *  This method is provided to allow renderers to inherit from this class,
         *  without having to reimplement the initializeRenderer() method, that uses
         *  both getLockCallback() and getUnlockCallback()
         *  \return         A pointer to the unlock function.
         *  \sa             getLockCallback()
         */
        virtual void*       getUnlockCallback();
        /**
         *  \brief              Lock callback for imem module
         *
         *  This callback will query the MainWorkflow for a frame or an audio sample
         *  \param  data        The callback data, this is most likely to be an EsHandler
         *  \param  dts         Unused, but provided by imem
         *  \param  pts         The pts for the buffer that will be provided
         *  \param  flags       Unused but provided by imem
         *  \param  bufferSize  The size of the buffer that will be provided
         *  \param  buffer      The buffer itself.
         */
        static int          lock( void *data, qint64 *dts, qint64 *pts,
                                quint32 *flags, size_t *bufferSize, void **buffer );
        /**
         *  \brief  "Subcallback", for video frame injection
         *
         *  \param  pts         The pts for the buffer that will be provided
         *  \param  bufferSize  The size of the buffer that will be provided
         *  \param  buffer      The buffer itself.
         */
        int                 lockVideo( qint64 *pts, size_t *bufferSize, void **buffer );
        /**
         *  \brief  "Subcallback", for audio sample injection
         *
         *  \param  pts         The pts for the buffer that will be provided
         *  \param  bufferSize  The size of the buffer that will be provided
         *  \param  buffer      The buffer itself.
         */
        int                 lockAudio( qint64 *pts, size_t *bufferSize, void **buffer );
        /**
         *  \brief  unlock callback for the imem module
         *
         *  \param  data        The callback data, this is most likely to be an EsHandler
         *  \param  buffSize    The size of the buffer
         *  \param  buffer      The buffer to be released
         */
        static void         unlock( void *data, size_t buffSize, void *buffer );
        /**
         *  \brief  Return the renderer specific width
         *
         *  The render width can changed depending on the render target.
         *  This getter will be used to initialized the MainWorkflow with the
         *  appropriate width.
         *  \return The video width for the underlying WorkflowRenderer
         */
        virtual quint32     width() const;
        /**
         *  \brief  Return the renderer specific height
         *
         *  The render width can changed depending on the render target.
         *  This getter will be used to initialized the MainWorkflow with the
         *  appropriate height.
         *  \return The video height for the underlying WorkflowRenderer
         */
        virtual quint32     height() const;
        /**
         *  \brief          Configure the production chain.
         */
        void                setupRenderer();

    protected:
        MainWorkflow*       m_mainWorkflow;
        LibVLCpp::Media*    m_media;
        bool                m_stopping;
        float               m_outputFps;
        unsigned char*	    m_renderVideoFrame;
        /**
         *  \brief          This isn't exactly the current PTS.
         *                  It's the number of frame rendered since the render has started.
         */
        qint64              m_pts;
        qint64              m_audioPts;

    private:
        /**
         *  \brief          When there's no sound to play, this is the buffer that'll
         *                  be injected
         */
        static quint8*      silencedAudioBuffer;
        size_t              m_videoBuffSize;
        EsHandler*          m_videoEsHandler;
        EsHandler*          m_audioEsHandler;
        quint32             m_nbChannels;
        quint32             m_rate;
        /**
         *  \brief          Used in permanent rendering mode, to know is some operations
         *                  has to be performed.
         */
        qint64              m_oldLength;


    public slots:
        /**
         *  \brief          Unused from this renderer.
         */
        virtual void        setClip( Clip* ){}
        /**
         *  \brief          Unused from this renderer.
         */
        virtual void        setMedia( Media* ) {}
        /**
         *  \brief          Unused from this renderer.
         */
        void                mediaUnloaded( const QUuid& ) {}
        /**
         *  \brief          The current frame just changed because of the timeline cursor
         */
        void                timelineCursorChanged( qint64 newFrame );
        /**
         *  \brief          The current frame just changed because of the timeline ruler
         */
        void                rulerCursorChanged( qint64 newFrame );
        /**
         *  \brief          The current frame just changed because of the preview widget
         */
        void                previewWidgetCursorChanged( qint64 newFrame );
        /**
         *  \brief          Called when the workflow end is reached.
         *  \sa             stop();
         */
        void                __endReached();

    private slots:
        /**
         *  \brief          Used to launch "permanent playback", as soon as the length
         *                  first changed to a non zero value.
         *
         *  If the length comes to a 0 value again, the permanent playback will be stoped.
         */
        void                mainWorkflowLenghtChanged( qint64 newLength );

};

#endif // WORKFLOWRENDERER_H
