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

#include "ClipWorkflow.h"
#include "VLCMediaPlayer.h"

//THIS IS A JUNK FOR TESTING CLIP WORKFLOW

class   TrackWorkflow : public QObject
{
    Q_OBJECT

    public:
        TrackWorkflow();

        void                    startRender();
        unsigned char*          getOutput();
    private:
        ClipWorkflow*           m_currentClipWorkflow;
        QMutex*                 m_condMutex;
        QWaitCondition*         m_waitCondition;
        LibVLCpp::MediaPlayer*  m_mediaPlayer;

    public slots:
        void            addClip( Clip* );
};

#endif // TRACKWORKFLOW_H
