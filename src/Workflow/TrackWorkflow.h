/*****************************************************************************
 * TrackWorkflow.h : Will query the Clip workflow for each successive clip in the track
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

#ifndef TRACKWORKFLOW_H
#define TRACKWORKFLOW_H

#include "MainWorkflow.h"
#include "StackedBuffer.hpp"

#include <QObject>

class   ClipWorkflow;
class   LightVideoFrame;

class   QDomElement;
class   QDomElement;
template <typename T>
class   QList;
template <typename T, typename U>
class   QMap;
class   QMutex;
class   QReadWriteLock;
class   QWaitCondition;

//TODO: REMOVE THIS
#ifndef FPS
#define FPS     30
#endif

#ifndef MAX_TRACKS
#define MAX_TRACKS 64
#endif

class   TrackWorkflow : public QObject
{
    Q_OBJECT

    public:
        TrackWorkflow( unsigned int trackId, MainWorkflow::TrackType type );
        ~TrackWorkflow();

        void*                                   getOutput( qint64 currentFrame,
                                                           qint64 subFrame );
        qint64                                  getLength() const;
        void                                    stop();
        void                                    pause();
        void                                    unpause();
        void                                    moveClip( const QUuid& id, qint64 startingFrame );
        Clip*                                   removeClip( const QUuid& id );
        ClipWorkflow*                           removeClipWorkflow( const QUuid& id );
        void                                    addClip( Clip*, qint64 start );
        void                                    addClip( ClipWorkflow*, qint64 start );
        qint64                                  getClipPosition( const QUuid& uuid ) const;
        Clip*                                   getClip( const QUuid& uuid );

        //FIXME: this won't be reliable as soon as we change the fps from the configuration
        static const unsigned int               nbFrameBeforePreload = 60;

        void                                    save( QDomDocument& doc, QDomElement& trackNode ) const;
        void                                    clear();

        void                                    forceRepositionning();
        void                                    renderOneFrame();

        /**
         *  \sa     MainWorkflow::setFullSpeedRender();
         */
        void                                    setFullSpeedRender( bool val );

    private:
        void                                    computeLength();
        void*                                   renderClip( ClipWorkflow* cw, qint64 currentFrame,
                                                            qint64 start, bool needRepositioning,
                                                            bool renderOneFrame );
        void                                    preloadClip( ClipWorkflow* cw );
        void                                    stopClipWorkflow( ClipWorkflow* cw );
        bool                                    checkEnd( qint64 currentFrame ) const;
        void                                    adjustClipTime( qint64 currentFrame, qint64 start, ClipWorkflow* cw );
        void                                    releasePreviousRender();


    private:
        unsigned int                            m_trackId;

        QMap<qint64, ClipWorkflow*>             m_clips;

        /**
         *  \brief      The track length in frames.
        */
        qint64                                  m_length;

        /**
         *  \brief      If a clip was moved, we may have to force repositionning.
         *              If this flag is set to true, we will force it anyway.
         */
        bool                                    m_forceRepositionning;
        QMutex*                                 m_forceRepositionningMutex;
        bool                                    m_renderOneFrame;
        QMutex                                  *m_renderOneFrameMutex;

        QReadWriteLock*                         m_clipsLock;

        bool                                    m_paused;

        MainWorkflow::TrackType                 m_trackType;
        qint64                                  m_lastFrame;
        StackedBuffer<LightVideoFrame*>*                    m_videoStackedBuffer;
        StackedBuffer<AudioClipWorkflow::AudioSample*>*     m_audioStackedBuffer;

    signals:
        void                                    trackEndReached( unsigned int );
};

#endif // TRACKWORKFLOW_H
