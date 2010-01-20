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

#include "ActionStack.h"
#include "AudioClipWorkflow.h"
#include "GenericRenderer.h"
#include "StackedAction.h"
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
         */
        virtual void        togglePlayPause( bool forcePause );
        /**
         *  \brief Stop the renderer.
         *  \sa togglePlayPause( bool );
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
         *  \brief          Add a clip to the workflow
         *
         *  This method is a part of the asynchrone workflow manipulation methodes, which
         *  means it can interact with the workflow during a render, using StackedAction
         *  \param  clip        The clip to add in the workflow
         *  \param  trackId     The track id in which the clip has to be added
         *  \param  startingPos The position where the clip should be added
         *  \param  trackType   The type of the clip to add (Audio or Video)
         *  \sa     removeClip( const QUuid&, uint32_t, MainWorkflow::TrackType );
         */
        void                addClip( Clip* clip, uint32_t trackId, qint64 startingPos,
                                     MainWorkflow::TrackType trackType );
        /**
         *  \brief          Remove a clip from the timeline
         *
         *  This method is a part of the asynchrone workflow manipulation methodes, which
         *  means it can interact with the workflow during a render, using StackedAction
         *  \param  uuid        the uuid of the clip to remove
         *  \param  trackId     the id of the track containing the clip to remove.
         *  \param  trackType   the type of the track
         *  \sa     addClip( Clip*, uint32_t, qint64, MainWorkflow::TrackType );
         */
        void                removeClip( const QUuid& uuid, uint32_t trackId,
                                        MainWorkflow::TrackType trackType );
        /**
         *  \brief                  Will split a clip and return the created clip,
         *                          resulting of the split operation.
         *
         *  This method is a part of the asynchrone workflow manipulation methodes, which
         *  means it can interact with the workflow during a render, using StackedAction
         *  This method will call
         *  MainWorkflow::addClip( Clip*, unsigned int, qint64, MainWorkflow::TrackType )
         *  thus emitting
         *  MainWorkflow::clipAdded( Clip*, unsigned int, qint64, MainWorkflow::TrackType )
         *  signal. It will also call Clip::setBoundaries( qint64, qint64, bool ), thus
         *  emitting a Clip::lengthUpdated() signal
         *  \param  toSplit         The clip to split
         *  \param  newClip         If the "toSplit" clip already has been splitted, this
         *                          is the clip resulting from the previous split
         *                          operation. This prevent creating and deleting clip
         *                          that could be used elsewhere.
         *  \param  trackId         The track containing the clip
         *  \param  newClipPos      The position of the "newClip" on the timeline.
         *  \param  newClipBegin    The starting frame (from the beginning of the clip's
         *                          parent media)
         *  \param  trackType       The track type (audio or video)
         *  \return                 The newly created clip if "newClip" was NULL;
         *                          else, newClip is returned.
         *  \sa                     MainWorkflow::addClip( Clip*, unsigned int, qint64,
                                                            MainWorkflow::TrackType )
         *  \sa                     MainWorkflow::clipAdded( Clip*, unsigned int, qint64,
         *                                                  MainWorkflow::TrackType )
         *  \sa                     Clip::setBoundaries( qint64, qint64, bool )
         *  \sa                     Clip::lengthUpdated()
         */
        Clip*               split( Clip* toSplit, Clip* newClip, uint32_t trackId,
                                   qint64 newClipPos, qint64 newClipBegin,
                                   MainWorkflow::TrackType trackType );
        /**
         *  \brief      Unplit a clip
         *
         *  This method is a part of the asynchrone workflow manipulation methodes, which
         *  means it can interact with the workflow during a render, using StackedAction
         *  This will cause the generated ClipWorkflow to be deleted, but the Clip itself
         *  won't be deleted.
         *  This method will call
         *  removeClip(const QUuid&, uint32_t, MainWorkflow::TrackType ), thus emitting
         *  a MainWorkflow::clipRemoved( Clip*, unsigned int, MainWorkflow::TrackType );
         *  signal.
         *  \param  origin      The clip that has been split
         *  \param  splitted    The clip that resulted from the split
         *  \param  trackId     The id of the track that was containing the split clip
         *  \param  trackType   The type of the clip that was split.
         *  \sa                 MainWorkflow::removeClip( const QUuid&, unsigned int,
         *                      MainWorkflow::TrackType )
         *  \sa                 MainWorkflow::clipRemoved( Clip*, unsigned int,
         *                      MainWorkflow::TrackType );
         */
        void                unsplit( Clip* origin, Clip* splitted, uint32_t trackId,
                                     MainWorkflow::TrackType trackType );
        /**
         *  \brief      Resize a clip in the workflow
         *
         *  This method is a part of the asynchrone workflow manipulation methodes, which
         *  means it can interact with the workflow during a render.
         *  \param  clip            The clip to resize
         *  \param  newBegin        The new clip beginning (if changing the beginning)
         *  \param  newEnd          The new clip end (if changing the end)
         *  \param  newPos          If changed from the beginning, the clip has to be
         *                          moved. This is the position at which it will be
         *                          positioned
         *  \param  trackId         The track containing the clip to resize
         *  \param  trackType       The type of the clip to resize
         *  \param  undoRedoAction  If true, the potential move resulting from the resize
         *                          will be emmited to the GUI. If this is not an undo
         *                          redo action, the GUI is already aware of the move.
         */
        void                resizeClip( Clip* clip, qint64 newBegin, qint64 newEnd,
                                        qint64 newPos, uint32_t trackId,
                                        MainWorkflow::TrackType trackType,
                                        bool undoRedoAction = false );

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
        /**
         *  \brief          check for workflow related actions that has been stacked.
         */
        void                checkActions();

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

    protected:
        MainWorkflow*       m_mainWorkflow;
        LibVLCpp::Media*    m_media;
        bool                m_stopping;
        float               m_outputFps;

    private:
        unsigned char*	    m_renderVideoFrame;
        /**
         *  \brief          When there's no sound to play, this is the buffer that'll
         *                  be injected
         */
        static uint8_t*     silencedAudioBuffer;
        size_t              m_videoBuffSize;
        Action::Stack       m_actions;
        QMutex*             m_actionsMutex;
        QWaitCondition*     m_waitCond;
        EsHandler*          m_videoEsHandler;
        EsHandler*          m_audioEsHandler;
        /**
         *  \brief          This isn't exactly the current PTS.
         *                  It's the number of frame rendered since the render has started.
         */
        qint64              m_pts;
        qint64              m_audioPts;
        uint32_t            m_nbChannels;
        uint32_t            m_rate;


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
         *  \brief          called when the workflow is paused
         */
        void                mainWorkflowPaused();
        /**
         *  \brief          called when the workflow is unpaused
         */
        void                mainWorkflowUnpaused();

};

#endif // WORKFLOWRENDERER_H
