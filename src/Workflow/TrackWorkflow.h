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
        TrackWorkflow();

        void                                    startRender();
        unsigned char*                          getOutput( qint64 currentFrame );
        qint64                                  getLength() const;

        /**
            \brief  Set the track workflow position
            \param  pos: The new pos in VLC position
        */
        void                                    setPosition( float pos );

        //FIXME: this won't be reliable as soon as we change the fps from the configuration
        static const unsigned int               nbFrameBeforePreload = 60;
        static unsigned char*                   blackOutput;

    private:
        /**
          * \brief      Check if there's a ClipWorkflow that's comming soon. If so, it preload it to avoid
                        freeze when switching video.
                        This does NOT search for the next current clip !
          * \return     true if at least one video remains, false otherwise (IE end of this track)
          */
        bool                                    checkNextClip( qint64 currentFrame );
        void                                    computeLength();

    private:
        QMap<qint64, ClipWorkflow*>             m_clips;
        /**
         *  \brief      An iterator that "point" the current ClipWorkflow used.
         *
         * This holds the current ClipWorkflow, and the current starting frame
         * of this ClipWorkflow.
         * If the track is empty at a T time, this iterator still points to the last
         * ClipWorkflow used. However, if the next video occurs to be the first one
         * in the Track, this iterators is equal to m_clips.end();
        */
        QMap<qint64, ClipWorkflow*>::iterator   m_current;
        QMutex*                                 m_condMutex;
        QReadWriteLock*                         m_currentLock;
        QWaitCondition*                         m_waitCondition;
        LibVLCpp::MediaPlayer*                  m_mediaPlayer;
        /**
            \brief      The track length in frames.
        */
        qint64                                  m_length;

    public:
        void            addClip( Clip*, qint64 start );
};

#endif // TRACKWORKFLOW_H
