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

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QMap>
#include <QList>
#include <QReadWriteLock>

#include "ClipWorkflow.h"
#include "VLCMediaPlayer.h"

//TODO: REMOVE THIS
#ifndef FPS
#define FPS     30
#endif

class   TrackWorkflow : public QObject
{
    Q_OBJECT

    public:
        TrackWorkflow( unsigned int trackId );
        ~TrackWorkflow();

        unsigned char*                          getOutput( qint64 currentFrame );
        qint64                                  getLength() const;
        void                                    stop();

        //FIXME: this won't be reliable as soon as we change the fps from the configuration
        static const unsigned int               nbFrameBeforePreload = 60;

    private:
        void                                    computeLength();
        unsigned char*                          renderClip( ClipWorkflow* cw, qint64 currentFrame,
                                                            qint64 start, bool needRepositioning );
        void                                    preloadClip( ClipWorkflow* cw );
        void                                    stopClipWorkflow( ClipWorkflow* cw );
        bool                                    checkEnd( qint64 currentFrame ) const;

    private:
        unsigned int                            m_trackId;

        QMap<qint64, ClipWorkflow*>             m_clips;

        /**
         *  This is the MediaPlayer that the clipworkflow
         *  will be using to process its render.
         *  It is never used internally.
         */
        LibVLCpp::MediaPlayer*                  m_mediaPlayer;
        /**
         *  \brief      The track length in frames.
        */
        qint64                                  m_length;

    public:
        void            addClip( Clip*, qint64 start );

    signals:
        void            trackEndReached( unsigned int );
};

#endif // TRACKWORKFLOW_H
