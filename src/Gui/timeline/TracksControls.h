/*****************************************************************************
 * TracksControls.cpp: Left panel of the timeline
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Ludovic Fauvet <etix@l0cal.com>
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

#ifndef TRACKSCONTROLS_H
#define TRACKSCONTROLS_H

#include <QScrollArea>
#include <QVBoxLayout>
#include "GraphicsTrack.hpp"
#include "TrackControls.h"

/**
 * \brief Tracks controller widget
 */
class TracksControls : public QScrollArea
{
    Q_OBJECT
public:
    TracksControls( QWidget* parent = 0 );

public slots:
    /**
     * \brief Add a new controller for a video track.
     * \param track Pointer to the newly added track.
     */
    void addVideoTrack( GraphicsTrack* track );
    /**
     * \brief Add a new controller for an audio track.
     * \param track Pointer to the newly added track.
     */
    void addAudioTrack( GraphicsTrack* track );
    /**
     * \brief Remove all controllers from the widget.
     */
    void clear();

private:
    QWidget* m_centralWidget;
    QWidget* m_separator;
    QVBoxLayout* m_layout;
};

#endif // TRACKSCONTROLS_H
