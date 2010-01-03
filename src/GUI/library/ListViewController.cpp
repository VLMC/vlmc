/*****************************************************************************
 * ListViewController.h:
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

#include <QtDebug>
#include <QPushButton>

#include "ListViewController.h"
#include "MediaCellView.h"
#include "Library.h"

ListViewController::ListViewController( StackViewController* nav ) : m_nav( nav )
{
    m_title      = new QString( "Media List" );
    m_scrollArea = new QScrollArea();
    m_layout     = new QVBoxLayout();

    m_layout->setAlignment( Qt::AlignTop );
    m_layout->setSpacing( 0 );
    m_layout->setContentsMargins( 0, 0, 0, 0 );
    m_scrollArea->setLayout( m_layout );
    m_scrollArea->setWidgetResizable( true );
    m_scrollArea->setAcceptDrops( true );

}

ListViewController::~ListViewController()
{
    delete m_title;
    delete m_scrollArea;
    delete m_layout;
}

// ViewController method implementation

const QString&      ListViewController::title() const
{
    return *m_title;
}

QWidget*            ListViewController::view() const
{
    return m_scrollArea;
}

void                ListViewController::addCell( QWidget* cell )
{
    m_layout->addWidget( cell );
}

void            ListViewController::removeCell( QWidget* cell )
{
    m_layout->removeWidget( cell );
    delete cell;
}
