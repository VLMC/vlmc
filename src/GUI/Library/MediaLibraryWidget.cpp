/*****************************************************************************
 * MediaLibraryWidget.cpp
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Thomas Boquet <thomas.boquet@gmail.com>
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

#include "MediaLibraryWidget.h"

MediaLibraryWidget::MediaLibraryWidget( QWidget* parent ) : QWidget( parent )
{
    m_nav = new StackViewController( this );
    MediaListViewController* list = new MediaListViewController( m_nav );
    connect( list, SIGNAL( mediaSelected( Media* ) ), this, SLOT( mediaSelection( Media* ) ) );

    m_nav->pushViewController( list );
}

MediaLibraryWidget::~MediaLibraryWidget()
{
    delete m_nav;
}

const ViewController*       MediaLibraryWidget::getCurrentViewController()
{
    return m_nav->getCurrentViewController();
}

void    MediaLibraryWidget::mediaSelection( Media* media )
{
    emit mediaSelected( media );
}

void    MediaLibraryWidget::clipSelection( Clip* clip )
{
    emit clipSelected( clip );
}
