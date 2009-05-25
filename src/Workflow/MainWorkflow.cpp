/*****************************************************************************
 * MainWorkflow.cpp : Will query all of the track workflows to render the final
 *                    image
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

#include <QtDebug>

#include "MainWorkflow.h"

MainWorkflow::MainWorkflow()
{
    m_tracks = new TrackWorkflow*[NB_TRACKS];
    for (unsigned int i = 0; i < NB_TRACKS; ++i)
        m_tracks[i] = new TrackWorkflow;
}

void    MainWorkflow::addClip( Clip* clip, unsigned int trackId )
{
    Q_ASSERT_X( trackId < NB_TRACKS, "MainWorkflow::addClip",
                "The specified trackId isn't valid, for it's higher than the number of tracks");

    qDebug() << "MainWorkflow: Adding clip" << clip->getUuid() << "to track" << trackId;
    m_tracks[trackId]->addClip( clip );
}

void    MainWorkflow::startRender()
{
    m_tracks[0]->startRender();
}

unsigned char*    MainWorkflow::getOutput()
{
    return m_tracks[0]->getOutput();
}
