/*****************************************************************************
 * ImportMediaCellView.cpp: Inherited class of the MediaCellView
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Geoffroy Lacarriere <geoffroylaca@gmail.com>
 *          Thomas Boquet <thomas.boquet@gmail.com>
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

#include "ImportMediaCellView.h"

ImportMediaCellView::ImportMediaCellView( const QUuid& uuid, QWidget *parent ) : MediaCellView( uuid, parent )
{
}

void    ImportMediaCellView::mouseDoubleClickEvent( QMouseEvent* )
{
}

void    ImportMediaCellView::mousePressEvent( QMouseEvent* )
{
    emit cellSelected( uuid() );
    this->focusWidget();
}

void    ImportMediaCellView::mouseMoveEvent( QMouseEvent* )
{
}

