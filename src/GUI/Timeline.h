/*****************************************************************************
 * Timeline.h: Widget that handle the tracks
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

#ifndef TIMELINE_H
#define TIMELINE_H

#include "vlmc.h"
#include "ui_Timeline.h"
#include "TracksRuler.h"
#include "Workflow/MainWorkflow.h"

class TracksScene;
class TracksView;

class Timeline : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY( Timeline )
public:
    explicit Timeline( QWidget *parent = 0 );
    virtual ~Timeline();
    TracksView*         tracksView() { return m_tracksView; }
    TracksScene*        tracksScene() { return m_tracksScene; }
    TracksRuler*        tracksRuler() { return m_tracksRuler; }
    static Timeline*    getInstance() { return m_instance; }

public slots:
    void changeZoom( int factor );
    void setDuration( int duration );
    void setTool( ToolButtons button );
    void actionAddClip( Clip* clip, unsigned int track, qint64 start, MainWorkflow::TrackType );
    void actionMoveClip( const QUuid& uuid, unsigned int track, qint64 time, MainWorkflow::TrackType );
    void actionRemoveClip( const QUuid& uuid, unsigned int track, MainWorkflow::TrackType );

protected:
    virtual void changeEvent( QEvent *e );

private:
    Ui::Timeline        m_ui;
    TracksView*         m_tracksView;
    TracksScene*        m_tracksScene;
    TracksRuler*        m_tracksRuler;
    double              m_scale;
    MainWorkflow*       m_mainWorkflow;
    static Timeline*    m_instance;
};

#endif // TIMELINE_H
