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

#define MAX_NAME_LENGTH 10

ListViewMediaItem::ListViewMediaItem( Clip* clip, Media::FileType fType, QListWidget* parent, int type ) :
        QListWidgetItem( parent, type ), m_clip ( clip )
{
    m_fileType = fType;

    setIcon( QIcon( ":/images/images/vlmc.png") );

    m_truncatedName = clip->getParent()->getFileInfo()->baseName();
    if ( m_truncatedName.length() > MAX_NAME_LENGTH )
    {
        m_truncatedName.truncate( MAX_NAME_LENGTH - 3 );
        m_truncatedName += "...";
    }
    setText( m_truncatedName );

    setIcon( QIcon( m_clip->getParent()->getSnapshot() ) );
    QTime   length;
    length = length.addSecs( m_clip->getLength() / 1000 );

    setToolTip( "Filename: " + m_clip->getParent()->getFileInfo()->fileName() + "\n" +
                "Length: " + length.toString() );
}

ListViewMediaItem::~ListViewMediaItem()
{

}

Clip*      ListViewMediaItem::getClip()
{
    return m_clip;
}



