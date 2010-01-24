/*****************************************************************************
 * GraphicsTrack.h: Graphically represent a track in the timeline
 *****************************************************************************
 * Copyright (C) 2008-2010 the VLMC team
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

#ifndef GRAPHICSTRACK_H
#define GRAPHICSTRACK_H

#include <QGraphicsWidget>
#include <QPainter>
#include <QDebug>
#include "MainWorkflow.h"
#include "Timeline.h"

class GraphicsTrack : public QGraphicsWidget
{
    Q_OBJECT

public:
    enum { Type = UserType + 2 };

    GraphicsTrack( MainWorkflow::TrackType type, quint32 trackNumber,
                   QGraphicsItem *parent = 0 );

    void setHeight( int height );
    int height();
    void setTrackEnabled( bool enabled );
    bool trackEnabled();
    quint32 trackNumber();
    MainWorkflow::TrackType mediaType();
    virtual int type() const { return Type; }

private:
    MainWorkflow::TrackType m_type;
    quint32 m_trackNumber;
    bool m_enabled;
};

#endif // GRAPHICSTRACK_H
