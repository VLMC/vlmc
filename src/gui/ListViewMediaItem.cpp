/*****************************************************************************
 * ListViewMediaItem.cpp: VLMC LibraryWidget
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Christophe Courtaut <christophe.courtaut@gmail.com>
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

#include "ListViewMediaItem.h"
#include <QPixmap>
#include <QDebug>

ListViewMediaItem::ListViewMediaItem( QFileInfo* fInfo, ListViewMediaItem::fType fType, QListWidget* parent, int type ) :
        QListWidgetItem( parent, type ), m_fileInfo( NULL ), m_clip( NULL )
{
    m_fileInfo = fInfo;
    m_fileType = fType;

    setIcon( QIcon( ":/images/images/vlmc.png") );
    setText( fInfo->baseName() );

    m_renderWidget = new QWidget();

    //TODO: create the actual clip.
}

ListViewMediaItem::~ListViewMediaItem()
{
    if ( m_fileInfo != NULL )
        delete m_fileInfo;
    if ( m_clip != NULL )
        delete m_clip;
}
