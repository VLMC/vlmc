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
    m_nbMediasToLoad = -1;
}

Media*          Library::getMedia( const QUuid& uuid )
{
    return getElementByUuid( m_medias, uuid );
}

Clip*           Library::getClip( const QUuid& uuid )
{
    return getElementByUuid( m_clips, uuid );
}

void        Library::removingMediaAsked( const QUuid& uuid )
{
    QHash<QUuid, Media*>::iterator   it = m_medias.find( uuid );
    if ( it == m_medias.end() )
        return ;
    Media*  media = it.value();
    emit mediaRemoved( uuid );
    //TODO: this is obviously a memleak, but at the moment, the library depends on the clip to work,
    //and won't be abble to remove the ListViewMediaItem without it.
    //delete *it;
    m_medias.erase( it );
    delete media;
}

void        Library::metaDataComputed( Media* media )
{
    emit newMediaLoaded( media );
    Clip* clip = new Clip( media );
    m_clips[media->getUuid()] = clip;
    emit newClipLoaded( clip );
    if ( m_nbMediasToLoad >= 0 )
    {
        m_nbMediasToLoad.fetchAndAddAcquire( -1 );
        if ( m_nbMediasToLoad <= 0 )
        {
            emit projectLoaded();
        }
    }
}

void        Library::newMediaLoadingAsked( const QString& filePath, const QString& uuid )
{
    Media*   media;
    if ( mediaAlreadyLoaded( filePath ) == true )
        return ;
    media = new Media( filePath, uuid );
    m_medias[media->getUuid()] = media;
    connect( media, SIGNAL( metaDataComputed( Media* ) ), this, SLOT( metaDataComputed( Media* ) ), Qt::DirectConnection );
    emit metadataRequired( media );
}

void        Library::addMedia( Media* media )
{
    QUuid id;
    foreach( id, m_medias.keys() )
        if ( m_medias.value( id )->getFileInfo()->filePath() == media->getFileInfo()->filePath() )
        {
            delete media;
            return;
        }
    m_medias[media->getUuid()] = media;
    metaDataComputed( media );
}

bool        Library::mediaAlreadyLoaded( const QString& filePath )
{
    //FIXME: Is this necessary ??
    Media*   media;
    foreach ( media, m_medias )
    {
        if ( media->getFileInfo()->absoluteFilePath() == filePath )
            return true;
    }
    return false;
}

void        Library::loadProject( const QDomElement& medias )
{
    if ( medias.isNull() == true || medias.tagName() != "medias" )
    {
        qWarning() << "Invalid medias node";
        return ;
    }

    QDomElement elem = medias.firstChild().toElement();
    m_nbMediasToLoad = 0;
    while ( elem.isNull() == false )
    {
        QDomElement mediaProperty = elem.firstChild().toElement();
        QString     path;
        QString     uuid;

        while ( mediaProperty.isNull() == false )
        {
            QString tagName = mediaProperty.tagName();
            if ( tagName == "path" )
                path = mediaProperty.text();
            else if ( tagName == "uuid" )
                uuid = mediaProperty.text();
            else
                qWarning() << "Unknown field" << tagName;
            mediaProperty = mediaProperty.nextSibling().toElement();
        }
        //FIXME: This is verry redondant...
        if ( mediaAlreadyLoaded( path ) == true )
        {
            Media*   media;
            QHash<QUuid, Media*>::iterator   it = m_medias.begin();
            QHash<QUuid, Media*>::iterator   end = m_medias.end();

            for ( ; it != end; ++it )
            {
                if ( it.value()->getFileInfo()->absoluteFilePath() == path )
                {
                    media = it.value();
                    media->setUuid( QUuid( uuid ) );
                    m_medias.erase( it );
                    m_medias[media->getUuid()] = media;
                    break ;
                }
            }
        }
        else
        {
            m_nbMediasToLoad.fetchAndAddAcquire( 1 );
            newMediaLoadingAsked( path, uuid );
        }

        elem = elem.nextSibling().toElement();
    }
}

void        Library::saveProject( QDomDocument& doc, QDomElement& rootNode )
{
    QHash<QUuid, Media*>::iterator          it = m_medias.begin();
    QHash<QUuid, Media*>::iterator          end = m_medias.end();

    QDomElement medias = doc.createElement( "medias" );

    for ( ; it != end; ++it )
    {
        QDomElement media = doc.createElement( "media" );

        medias.appendChild( media );
        QDomElement mrl = doc.createElement( "path" );

        QDomCharacterData   text = doc.createTextNode( it.value()->getFileInfo()->absoluteFilePath() );

        QDomElement uuid = doc.createElement( "uuid" );
        QDomCharacterData   text2 = doc.createTextNode( it.value()->getUuid().toString() );

        mrl.appendChild( text );
        uuid.appendChild( text2 );

        media.appendChild( mrl );
        media.appendChild( uuid );
    }
    rootNode.appendChild( medias );
}
