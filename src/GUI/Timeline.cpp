/*****************************************************************************
 * Timeline.cpp: Widget that handle the tracks
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

#include <QHBoxLayout>
#include <QScrollBar>
#include <QtDebug>
#include "Timeline.h"
#include "TracksView.h"
#include "TracksScene.h"
#include "TracksControls.h"
#include "TracksRuler.h"
#include "WorkflowRenderer.h"

Timeline*   Timeline::m_instance = NULL;

Timeline::Timeline( WorkflowRenderer* renderer, QWidget *parent ) :
    QWidget( parent ), m_scale( 1.0 ), m_renderer( renderer )
{
    Q_ASSERT( m_instance == NULL );
    m_instance = this;
    m_ui.setupUi( this );

    m_mainWorkflow = MainWorkflow::getInstance();

    m_tracksScene = new TracksScene( this );
    m_tracksView = new TracksView( m_tracksScene, m_mainWorkflow, m_renderer, m_ui.tracksFrame );
    m_tracksView->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    m_tracksView->scale(1, 1);

    QHBoxLayout* tracksViewLayout = new QHBoxLayout();
    tracksViewLayout->setContentsMargins( 0, 0, 0, 0 );
    m_ui.tracksFrame->setLayout( tracksViewLayout );
    tracksViewLayout->addWidget( m_tracksView );

    m_tracksRuler = new TracksRuler( tracksView(), this );
    QHBoxLayout* tracksRulerLayout = new QHBoxLayout();
    tracksRulerLayout->setContentsMargins( 0, 0, 0, 0 );
    m_ui.rulerFrame->setLayout( tracksRulerLayout );
    tracksRulerLayout->addWidget( m_tracksRuler );

    m_tracksControls = new TracksControls( this );
    QHBoxLayout* tracksControlsLayout = new QHBoxLayout();
    tracksControlsLayout->setContentsMargins( 0, 0, 0, 0 );
    m_ui.controlsFrame->setLayout( tracksControlsLayout );
    tracksControlsLayout->addWidget( m_tracksControls );


    changeZoom( 10 );
    setDuration( 0 );

    // Scroll
    connect( m_tracksView->horizontalScrollBar(), SIGNAL( valueChanged(int) ),
             m_tracksRuler, SLOT( moveRuler(int) ) );
    connect( m_tracksView->verticalScrollBar(), SIGNAL( valueChanged(int) ),
             m_tracksControls->verticalScrollBar(), SLOT( setValue(int) ) );
    connect( m_tracksControls->verticalScrollBar(), SIGNAL( valueChanged(int) ),
             m_tracksView->verticalScrollBar(), SLOT( setValue(int) ) );

    // Project duration change
    connect( m_tracksView, SIGNAL( durationChanged(int) ), this, SLOT( setDuration(int) ) );

    // Clip actions
    connect( m_mainWorkflow, SIGNAL( clipAdded(Clip*,uint,qint64,MainWorkflow::TrackType ) ),
             this, SLOT( actionAddClip(Clip*,uint,qint64,MainWorkflow::TrackType ) ) );
    connect( m_mainWorkflow, SIGNAL( clipMoved(QUuid, uint, qint64,MainWorkflow::TrackType ) ),
             this, SLOT( actionMoveClip(QUuid,uint,qint64,MainWorkflow::TrackType ) ) );
    connect( m_mainWorkflow, SIGNAL( clipRemoved(Clip*,uint,MainWorkflow::TrackType ) ),
             this, SLOT( actionRemoveClip(Clip*,uint,MainWorkflow::TrackType )) );

    // Clear / reset
    connect( m_mainWorkflow, SIGNAL( cleared() ), m_tracksControls, SLOT( clear() ) );
    connect( m_mainWorkflow, SIGNAL( cleared() ), tracksView(), SLOT( clear() ) );

    // Tracks controls
    connect( m_tracksView, SIGNAL( videoTrackAdded(GraphicsTrack*) ),
             m_tracksControls, SLOT( addVideoTrack(GraphicsTrack*) ) );
    connect( m_tracksView, SIGNAL( audioTrackAdded(GraphicsTrack*) ),
             m_tracksControls, SLOT( addAudioTrack(GraphicsTrack*) ) );

    // Frames updates
    connect( m_renderer, SIGNAL( frameChanged(qint64, MainWorkflow::FrameChangedReason) ),
             m_tracksView->tracksCursor(), SLOT( frameChanged( qint64, MainWorkflow::FrameChangedReason ) ),
             Qt::QueuedConnection );
    connect( m_renderer, SIGNAL( frameChanged(qint64,MainWorkflow::FrameChangedReason) ),
             m_tracksRuler, SLOT( update() ) );
    connect( m_tracksRuler, SIGNAL( frameChanged(qint64,MainWorkflow::FrameChangedReason) ),
             m_renderer, SLOT( rulerCursorChanged(qint64)) );

    // Cursor position updates
    connect( m_tracksView->tracksCursor(), SIGNAL( cursorPositionChanged( qint64 ) ),
             m_renderer, SLOT( timelineCursorChanged(qint64) ) );

    m_tracksView->createLayout();
}

Timeline::~Timeline()
{
    MainWorkflow::destroyInstance();
}

void Timeline::changeEvent( QEvent *e )
{
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        m_ui.retranslateUi( this );
        break;
    default:
        break;
    }
}

void Timeline::clear()
{
    // The main workflow will ask the GUI to clear itself.
    m_mainWorkflow->clear();
}

void Timeline::changeZoom( int factor )
{
    m_tracksRuler->setPixelPerMark( factor );
    m_scale = (double) FRAME_SIZE / m_tracksRuler->comboScale[factor];
    m_tracksView->setScale( m_scale );
}

void Timeline::setDuration( int duration )
{
    m_tracksView->setDuration( duration );
    m_tracksRuler->setDuration( duration );
}

void Timeline::setTool( ToolButtons button )
{
    tracksView()->setTool( button );
}

void Timeline::actionAddClip( Clip* clip, unsigned int track, qint64 start, MainWorkflow::TrackType )
{
    tracksView()->addMediaItem( clip, track, start );
}

void Timeline::actionMoveClip( const QUuid& uuid, unsigned int track, qint64 time, MainWorkflow::TrackType )
{
    tracksView()->moveMediaItem( uuid, track, time );
    Q_ASSERT( tracksView()->setItemOldTrack( uuid, track ) == true );
    tracksView()->updateDuration();
    tracksRuler()->update();
}

void Timeline::actionRemoveClip( Clip* clip, unsigned int track, MainWorkflow::TrackType )
{
    tracksView()->removeMediaItem( clip->getUuid(), track );
}
