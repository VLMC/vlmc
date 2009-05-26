/*****************************************************************************
 * MainWorkflow.h : Will query all of the track workflows to render the final
 *                  image
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

#ifndef MAINWORKFLOW_H
#define MAINWORKFLOW_H

#include <QObject>

#include "TrackWorkflow.h"

//TODO: THIS HAS TO GO ASAP !!!!!
#define NB_TRACKS   1

class   MainWorkflow : public QObject
{
    Q_OBJECT

    public:
        MainWorkflow();

        void                    addClip( Clip* clip, unsigned int trackId, qint64 start );
        void                    startRender();
        unsigned char*          getOutput();

    private:
        TrackWorkflow**         m_tracks;
};

#endif // MAINWORKFLOW_H
