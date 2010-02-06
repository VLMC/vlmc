/*****************************************************************************
 * TracksControls.cpp: Left panel of the timeline
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
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

#include <QLabel>
#include "TracksControls.h"

TracksControls::TracksControls( QWidget* parent )
        : QScrollArea( parent )
{
    // Never show the scrollbars
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    setContentsMargins( 0, 0, 0, 0 );
    setFrameStyle( QFrame::NoFrame );

    m_centralWidget = new QWidget();
    m_centralWidget->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );

    m_layout = new QVBoxLayout;
    m_layout->setSizeConstraint( QLayout::SetMinimumSize );
    m_layout->setContentsMargins( 0, 0, 0, 0 );
    m_layout->setSpacing( 0 );

    m_separator = new QWidget;
    m_separator->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
    m_separator->setMinimumHeight( 20 );
    m_separator->setMaximumHeight( 20 );
    m_layout->addWidget( m_separator );

    m_centralWidget->setLayout( m_layout );
    setWidget( m_centralWidget );
}

void TracksControls::addVideoTrack( GraphicsTrack* track )
{
    TrackControls* item = new TrackControls( track, m_centralWidget );
    item->setMinimumWidth( 108 );
    item->setMinimumHeight( 25 );
    item->setContentsMargins( 0, 0, 0, 0 );
    m_layout->insertWidget( 0, item );
}

void TracksControls::addAudioTrack( GraphicsTrack* track )
{
    TrackControls* item = new TrackControls( track, m_centralWidget );
    item->setMinimumWidth( 108 );
    item->setMinimumHeight( 25 );
    item->setContentsMargins( 0, 0, 0, 0 );
    m_layout->insertWidget( -1, item );
}

void TracksControls::removeVideoTrack()
{
    QLayoutItem* item = m_layout->takeAt( 0 );
    if ( item->widget() )
        delete item->widget();
    delete item;

    // Force the central widget to shrink
    m_layout->activate();
    m_centralWidget->setGeometry( 0, 0, 1, 1 );
}

void TracksControls::removeAudioTrack()
{
    QLayoutItem* item = m_layout->takeAt( m_layout->count() - 1 );
    if ( item->widget() )
        delete item->widget();
    delete item;

    // Force the central widget to shrink
    m_layout->activate();
    m_centralWidget->setGeometry( 0, 0, 1, 1 );
}

void TracksControls::clear()
{
    m_layout->removeWidget( m_separator );
    QLayoutItem* child;
    while ( ( child = m_layout->takeAt( 0 ) ) != 0 )
    {
        if ( child->widget() )
            delete child->widget();
        delete child;
    }
    m_layout->addWidget( m_separator );

    // Force the central widget to shrink
    m_layout->activate();
    m_centralWidget->setGeometry( 0, 0, 1, 1 );
}
