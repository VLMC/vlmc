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
#include <QTime>

ListViewMediaItem::ListViewMediaItem( const Media* media, Library::FileType fType, QListWidget* parent, int type ) :
        QListWidgetItem( parent, type ), m_media ( media )
{
    m_fileType = fType;

    setIcon( QIcon( ":/images/images/vlmc.png") );
    setText( media->getFileInfo()->baseName() );

    connect( media, SIGNAL( snapshotChanged() ), this, SLOT( snapshotChanged() ) );
}

ListViewMediaItem::~ListViewMediaItem()
{

}

void        ListViewMediaItem::snapshotChanged()
{
    setIcon( QIcon( m_media->getSnapshot() ) );
    //this is a nasty patch for the moment :
    //TODO: fix this !!!!
    QTime   length;
    length = length.addSecs( m_media->getLength() / 1000 );

    setToolTip( "Filename: " + m_media->getFileInfo()->fileName() + "\n" +
                "Length: " + length.toString() );
}

const Media*     ListViewMediaItem::getMedia() const
{
    return m_media;
}



