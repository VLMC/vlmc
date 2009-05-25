/*****************************************************************************
 * TrackWorkflow.cpp : Will query the Clip workflow for each successive clip in the track
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

#include "TrackWorkflow.h"

TrackWorkflow::TrackWorkflow()
{
    m_condMutex = new QMutex;
    m_waitCondition = new QWaitCondition;
    m_mediaPlayer = new LibVLCpp::MediaPlayer();
}

void    TrackWorkflow::addClip( Clip* clip )
{
    m_currentClipWorkflow = new ClipWorkflow( clip, m_condMutex, m_waitCondition );
}

void    TrackWorkflow::startRender()
{
    m_currentClipWorkflow->startRender( m_mediaPlayer );
    while ( m_currentClipWorkflow->isReady() == false )
        usleep( 150 );
}

unsigned char*    TrackWorkflow::getOutput()
{
//    qDebug() << "Awaking all renderers";
    m_waitCondition->wakeAll();
    while ( m_currentClipWorkflow->renderComplete() == false )
    {
        //qDebug() << "Frame is not ready yet... waiting";
//        usleep( 1000 );
    }
//    qDebug() << "Frame rendered";
    if ( m_currentClipWorkflow->isEndReached() == true )
        return NULL;
    return m_currentClipWorkflow->getOutput();
}
