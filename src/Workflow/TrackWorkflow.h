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

#include "ClipWorkflow.h"
#include "VLCMediaPlayer.h"

#define FPS     30

class   TrackWorkflow : public QObject
{
    Q_OBJECT

    public:
        TrackWorkflow();

        void                                    startRender();
        unsigned char*                          getOutput();
        //FIXME: this won't be reliable as soon as we change the fps from the configuration
        static const unsigned int               nbFrameBeforePreload = 60; //We load aproximatively 2s before the frame has to render.        
        static unsigned char*                   blackOutput;

    private:
        /**
          * \return true if at least one video remains, false otherwise (IE end of this track)
          */
        bool                                    checkNextClip();

    private:
        QMap<qint64, ClipWorkflow*>             m_clips;
        QMap<qint64, ClipWorkflow*>::iterator   m_current;
        qint64                                  m_currentFrame;
        QMutex*                                 m_condMutex;
        QWaitCondition*                         m_waitCondition;
        LibVLCpp::MediaPlayer*                  m_mediaPlayer;
        bool                                    m_isRendering;

    public:
        void            addClip( Clip*, qint64 start );
};

#endif // TRACKWORKFLOW_H
