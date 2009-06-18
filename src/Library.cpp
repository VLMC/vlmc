/*****************************************************************************
 * Library.cpp: Multimedia library
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
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

/** \file
  * This file the library contains class implementation.
  * It's the the backend part of the Library widget of vlmc.
  * It can load and unload Medias (Medias.h/Media.cpp)
  */

#include <QtDebug>
#include "Library.h"

Library::Library()
{
}

Media*       Library::getMedia( const QUuid& uuid )
{
    QMutexLocker locker( &m_mutex );
    QHash<QUuid, Media*>::iterator   it = m_medias.find( uuid );
    if ( it == m_medias.end() )
        return NULL;
    return *it;
}

Media*      Library::getMedia( const QString& path )
{
    QMutexLocker locker( &m_mutex );
    QHash<QUuid, Media*>::iterator it;
    QHash<QUuid, Media*>::iterator ite = m_medias.end();
    for ( it = m_medias.begin(); it != ite; ++it )
        if ( ((Media*)it.value())->getFileInfo()->absoluteFilePath() == path )
            return it.value();
    return NULL;
}

void        Library::removingMediaAsked( const QUuid& uuid )
{
    QMutexLocker locker( &m_mutex );
    QHash<QUuid, Media*>::iterator   it = m_medias.find( uuid );
    if ( it == m_medias.end() )
        return ;
    emit mediaRemoved( uuid );
    //TODO: this is obviously a memleak, but at the moment, the library depends on the clip to work,
    //and won't be abble to remove the ListViewMediaItem without it.
    //delete *it;
    m_medias.erase( it );
}

void        Library::newMediaLoadingAsked( const QString& filePath )
{
    //Is this necessary ??
    QMutexLocker locker( &m_mutex );
    Media*   media;
    foreach ( media, m_medias )
    {
        if ( media->getFileInfo()->absoluteFilePath() == filePath )
            return ;
    }
    //TODO: maybe we should think about taking a reference to a QFileInfo on Clip::Clip() to avoid multiple new...
    QFileInfo* fInfo = new QFileInfo( filePath );
    media = new Media( fInfo );
    m_medias[media->getUuid()] = media;
    emit newMediaLoaded( media );
    delete fInfo;
}
