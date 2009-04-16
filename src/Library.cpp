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

#include <QtDebug>
#include "Library.h"
#include "Clip.h"

Library::Library()
{
}

Clip*       Library::getClip( const QUuid& uuid )
{
    QMutexLocker locker( &m_mutex );
    QHash<QUuid, Clip*>::iterator   it = m_clips.find( uuid );
    if ( it == m_clips.end() )
        return NULL;
    return *it;
}

void        Library::removingClipAsked( const QUuid& uuid )
{
    QMutexLocker locker( &m_mutex );
    QHash<QUuid, Clip*>::iterator   it = m_clips.find( uuid );
    if ( it == m_clips.end() )
        return ;
    emit clipRemoved( uuid );
    //TODO: this is obviously a memleak, but at the moment, the library depends on the clip to work,
    //and won't be abble to remove the ListViewMediaItem without it.
    //delete *it;
    m_clips.erase( it );
}

void        Library::newClipLoadingAsked( const QString& filePath )
{
    QMutexLocker locker( &m_mutex );
    Clip*   clip;
    foreach ( clip, m_clips )
    {
        if ( clip->getFileInfo()->absoluteFilePath() == filePath )
            return ;
    }
    //TODO: maybe we should think about taking a reference to a QFileInfo on Clip::Clip() to avoid multiple new...
    QFileInfo* fInfo = new QFileInfo( filePath );
    clip = new Clip( fInfo );
    m_clips[clip->getUuid()] = clip;
    emit newClipLoaded( clip );
    delete fInfo;
}
