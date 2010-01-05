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
#include "StackedAction.hpp"
#include "ActionStack.h"

class   WorkflowRenderer : public GenericRenderer
{
    Q_OBJECT
    Q_DISABLE_COPY( WorkflowRenderer )

    public:
        WorkflowRenderer();
        ~WorkflowRenderer();

        void                initializeRenderer();
        virtual void        togglePlayPause( bool forcePause );
        virtual void        stop();
        virtual void        nextFrame();
        virtual void        previousFrame();
        virtual qint64      getLengthMs() const;
        virtual qint64      getCurrentFrame() const;
        virtual float       getFps() const;
        void                addClip( Clip* clip, uint32_t trackId, qint64 startingPos, MainWorkflow::TrackType trackType );
        void                removeClip( const QUuid& uuid, uint32_t trackId, MainWorkflow::TrackType trackType );
        /**
         *  \brief                  Will split a clip and return the created clip, resulting of the split operation.
         *  \param  toSplit         The clip to split
         *  \param  newClip         If the "toSplit" clip already has been splitted, this is the clip resulting
         *                          from the previous split operation. This prevent creating and deleting clip that could be used elsewhere.
         *  \param  trackId         The track containing the clip
         *  \param  newClipPos      The position of the "newClip" on the timeline.
         *  \param  newClipBegin    The starting frame (from the beginning of the clip's parent media)
         *  \param  trackType       The track type (audio or video)
         *  \return                 The newly created clip if "newClip" was NULL; else, newClip is returned.
         */
        Clip*               split( Clip* toSplit, Clip* newClip, uint32_t trackId, qint64 newClipPos, qint64 newClipBegin, MainWorkflow::TrackType trackType );
        void                unsplit( Clip* origin, Clip* splitted, uint32_t trackId, MainWorkflow::TrackType trackType );
        /**
         *  \param  undoRedoAction: if true, the potential move resulting from the resize will be emmited to the GUI.
         *                          if this is not an undo redo action, the GUI is already aware of the move.
         */
        void                resizeClip( Clip* clip, qint64 newBegin, qint64 newEnd, qint64 newPos,
                                        uint32_t trackId, MainWorkflow::TrackType trackType, bool undoRedoAction = false );

        static void*        lock( void* datas );
        static void*        lockAudio( void* datas );
        static void         unlock( void* datas );
    private:
        void                internalPlayPause( bool forcePause );
        virtual void        startPreview();
        void                checkActions();

    protected:
        virtual void*       getLockCallback();
        virtual void*       getUnlockCallback();

    protected:
        MainWorkflow*       m_mainWorkflow;
        LibVLCpp::Media*    m_media;
        bool                m_stopping;
        float               m_outputFps;

    private:
        unsigned char*	    m_renderVideoFrame;
        unsigned char*	    m_renderAudioSample;
        Action::Stack       m_actions;
        QMutex*             m_actionsMutex;
        QWaitCondition*     m_waitCond;

    public slots:
        virtual void        setClip( Clip* ){}
        virtual void        setMedia( Media* ) {}
        void                mediaUnloaded( const QUuid& ) {}
        void                timelineCursorChanged( qint64 newFrame );
        void                rulerCursorChanged( qint64 newFrame );
        void                previewWidgetCursorChanged( qint64 newFrame );

        void                __endReached();

    private slots:
        void                mainWorkflowPaused();
        void                mainWorkflowUnpaused();

};

#endif // WORKFLOWRENDERER_H
