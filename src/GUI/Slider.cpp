/*****************************************************************************
 * Slider.cpp: Enhanced slider for user interactions
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

#include "Slider.h"

Slider::Slider( QWidget* parent ) : QSlider( parent ), m_isSliding( false )
{
    connect( this, SIGNAL( valueChanged(int) ), this, SLOT( sliderChanged(int) ) );
}

void Slider::mousePressEvent( QMouseEvent* event )
{
    m_isSliding = true;
    if ( event->button() != Qt::LeftButton &&
         event->button() != Qt::MidButton )
    {
        QSlider::mousePressEvent( event );
    }

    QMouseEvent newEvent( event->type(), event->pos(), event->globalPos(),
        Qt::MouseButton( event->button() ^ Qt::LeftButton ^ Qt::MidButton ),
        Qt::MouseButtons( event->buttons() ^ Qt::LeftButton ^ Qt::MidButton ),
        event->modifiers() );
    QSlider::mousePressEvent( &newEvent );
}

void Slider::mouseReleaseEvent( QMouseEvent* event )
{
    m_isSliding = false;
    QSlider::mouseReleaseEvent( event );
}

void Slider::sliderChanged( int value )
{
    if ( m_isSliding )
    {
        emit sliderPosChanged( value );
    }
}
