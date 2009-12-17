/*****************************************************************************
 * MediaLibraryWidget.h
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

#include "MediaListViewController.h"

#include <QDebug>

MediaListViewController::MediaListViewController( StackViewController* nav ) :
        ListViewController( nav ), m_nav( nav )
{
    connect(Library::getInstance(), SIGNAL( newMediaLoaded( Media* ) ), this, SLOT( newMediaLoaded( Media* ) ) );
    m_cells = new QHash<QUuid, QWidget*>();
}

MediaListViewController::~MediaListViewController()
{
    delete m_cells;
}

void        MediaListViewController::newMediaLoaded( Media* media )
{
    MediaCellView* cell = new MediaCellView( media->getUuid() );

    connect( cell, SIGNAL ( cellSelected( QUuid ) ), this, SLOT ( cellSelection( QUuid ) ) );
    connect( cell, SIGNAL ( cellDeleted( QUuid ) ), this, SLOT( mediaDeletion( QUuid ) ) );

    if ( media->getMetadata() != Media::ParsedWithSnapshot )
        connect( media, SIGNAL( snapshotComputed( Media* ) ), this, SLOT( updateCell( Media* ) ) );
    cell->setThumbnail( media->getSnapshot() );
    cell->setTitle( media->getFileName() );
    cell->setLength( media->getLengthMS() );
    addCell(cell);
    m_cells->insert( media->getUuid(), cell );
}

void    MediaListViewController::cellSelection( const QUuid& uuid )
{
    if ( m_currentUuid == uuid )
        return;

    if ( m_cells->contains( uuid ) )
    {
        if ( !m_currentUuid.isNull() && m_cells->contains( m_currentUuid ) )
            m_cells->value( m_currentUuid )->setPalette( m_cells->value( uuid )->palette() );
        QPalette p = m_cells->value( uuid )->palette();
        p.setColor( QPalette::Window, QColor( Qt::darkBlue ) );
        m_cells->value( uuid )->setPalette( p );
        m_currentUuid = uuid;
        emit mediaSelected( Library::getInstance()->getMedia( uuid ) );
    }
}

void    MediaListViewController::mediaDeletion( const QUuid& uuid )
{
    emit mediaDeleted( uuid );
}

void    MediaListViewController::mediaRemoved( const QUuid& uuid )
{
    QWidget* cell = m_cells->value( uuid );
    removeCell( cell );
    m_cells->remove( uuid );
    delete cell;
    m_currentUuid = QUuid();
}

void    MediaListViewController::updateCell( Media* media )
{
    MediaCellView* cell = dynamic_cast<MediaCellView*>( m_cells->value( media->getUuid(), NULL ) );
    if ( cell != NULL )
        cell->setThumbnail( media->getSnapshot() );
}
