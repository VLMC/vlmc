/*****************************************************************************
 * TrackControls.h: Widget used to configure a track
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

#ifndef TRACKCONTROLS_H
#define TRACKCONTROLS_H

#include <QtGui/QWidget>
#include "timeline/GraphicsTrack.hpp"

namespace Ui {
    class TrackControls;
}

class TrackControls : public QWidget
{
    Q_OBJECT
public:
    TrackControls( GraphicsTrack* track, QWidget *parent = 0 );
    ~TrackControls();

protected:
    void changeEvent( QEvent *e );

private slots:
    void setTrackDisabled( bool disable );

private:
    Ui::TrackControls *m_ui;
    GraphicsTrack* m_track;
};

#endif // TRACKCONTROLS_H
