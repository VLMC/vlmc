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
#include "Timeline.h"
#include "TracksView.h"
#include "TracksScene.h"

Timeline::Timeline( QWidget *parent ) :
    QWidget( parent )
{
    m_ui.setupUi( this );

    m_tracksScene = new TracksScene( this );
    m_tracksView = new TracksView( m_tracksScene, m_ui.tracksFrame );
    m_tracksView->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    m_tracksView->scale(1, 1);
    m_tracksView->setAlignment( Qt::AlignLeft | Qt::AlignTop );

    QHBoxLayout* tracksViewLayout = new QHBoxLayout();
    tracksViewLayout->setContentsMargins( 0, 0, 0, 0 );
    m_ui.tracksFrame->setLayout( tracksViewLayout );
    tracksViewLayout->addWidget( m_tracksView );

    m_tracksRuler = new TracksRuler( this );
    QHBoxLayout* tracksRulerLayout = new QHBoxLayout();
    tracksRulerLayout->setContentsMargins( 0, 0, 0, 0 );
    m_ui.rulerFrame->setLayout( tracksRulerLayout );
    tracksRulerLayout->addWidget( m_tracksRuler );

    connect( m_tracksView->horizontalScrollBar(), SIGNAL( valueChanged( int ) ), m_tracksRuler, SLOT( moveRuler( int ) ) );
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
