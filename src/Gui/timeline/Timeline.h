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
#include "Workflow/MainWorkflow.h"

class TracksScene;
class TracksView;
class TracksControls;
class TracksRuler;
class WorkflowRenderer;

/**
 * \brief Entry point of the timeline widget.
 */
class Timeline : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY( Timeline )
public:
    explicit Timeline( WorkflowRenderer* renderer, QWidget *parent = 0 );
    virtual ~Timeline();
    /// Return a pointer to the TracksView instance.
    TracksView*         tracksView() { return m_tracksView; }
    /// Return a pointer to the TracksScene instance.
    TracksScene*        tracksScene() { return m_tracksScene; }
    /// Return a pointer to the TracksRuler instance.
    TracksRuler*        tracksRuler() { return m_tracksRuler; }
    /// Return a pointer to the Timeline instance (singleton).
    static Timeline*    getInstance() { return m_instance; }

public slots:
    /**
     * \brief Asks the workflow to clear itself.
     */
    void clear();
    /**
     * \brief Change the zoom level for all widgets of the timeline.
     * \param factor The zoom factor.
     */
    void changeZoom( int factor );
    /**
     * \brief Change the duration of the project.
     * \param duration Duration in frames.
     */
    void setDuration( int duration );
    /**
     * \brief Change the currently selected tool.
     */
    void setTool( ToolButtons button );
    /**
     * \brief Insert an item into the timeline.
     * \param clip Clip to insert.
     * \param track The track's number.
     * \param start The position in frames.
     * \param
     */
    void actionAddClip( Clip* clip, unsigned int track, qint64 start, MainWorkflow::TrackType );
    /**
     * \brief Move an item in the timeline.
     * \param uuid The Universally Unique Identifier of the item.
     * \param track The new track of the item.
     * \param time The new position (in frames) of the item.
     */
    void actionMoveClip( const QUuid& uuid, unsigned int track, qint64 time, MainWorkflow::TrackType );
    /**
     * \brief Remove an item from the timeline.
     * \param uuid The Universally Unique Identifier of the item.
     * \param track The current track of the item.
     */
    void actionRemoveClip( Clip* clip, unsigned int track, MainWorkflow::TrackType );

protected:
    virtual void changeEvent( QEvent *e );

private:
    Ui::Timeline        m_ui;
    TracksView*         m_tracksView;
    TracksScene*        m_tracksScene;
    TracksRuler*        m_tracksRuler;
    TracksControls*     m_tracksControls;
    double              m_scale;
    MainWorkflow*       m_mainWorkflow;
    WorkflowRenderer*   m_renderer;
    static Timeline*    m_instance;
};

#endif // TIMELINE_H
