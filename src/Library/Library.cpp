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

#include "Clip.h"
#include "Library.h"
#include "Media.h"
#include "MetaDataManager.h"

#include <QDebug>
#include <QDomElement>
#include <QHash>
#include <QMessageBox>
#include <QUuid>

Library::Library()
{
}

Media*
Library::getMedia( const QUuid& uuid )
{
    return getElementByUuid( m_medias, uuid );
}

Clip*
Library::getClip( const QUuid& uuid )
{
    Clip*   clip;
    clip = getElementByUuid( m_clips, uuid );

    return clip;
}

void
Library::removingMediaAsked( const QUuid& uuid )
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
    //delete media;
    deleteMedia( media );
}

void
Library::deleteMedia( Media *media )
{
    if ( media->getMetadata() == Media::ParsedWithAudioSpectrum )
        delete media;
    else
    {
        m_mediaToDelete.append( media );
        connect( media, SIGNAL( audioSpectrumComputed( Media* ) ), this, SLOT( deleteMedia( Media* ) ) );
    }
}

void
Library::audioSpectrumComputed( Media *media )
{
    disconnect( media, SIGNAL( audioSpectrumComputed( Media* ) ), this, SLOT( audioSpectrumComputed( Media* ) ) );
    if ( m_mediaToDelete.contains( media ) )
    {
        m_mediaToDelete.removeAll( media );
        delete media;
    }
}

void
Library::metaDataComputed( Media* media )
{
    emit newMediaLoaded( media );
    Clip* clip = new Clip( media );
    m_clips[media->getUuid()] = clip;
    //If the media have some clip, add then to m_clips
    const QHash<QUuid, Clip*>*    clips = media->clips();
    if ( clips->size() != 0 )
    {
        QHash<QUuid, Clip*>::const_iterator   it = clips->begin();
        QHash<QUuid, Clip*>::const_iterator   ed = clips->end();

        for ( ; it != ed; ++it )
            m_clips[it.key()] = it.value();
    }
}

void
Library::newMediaLoadingAsked( const QString& filePath, const QString& uuid )
{
    Media*   media;
    if ( mediaAlreadyLoaded( filePath ) == true )
        return ;
    media = new Media( filePath, uuid );
    m_medias[media->getUuid()] = media;
    connect( media, SIGNAL( metaDataComputed( Media* ) ), this, SLOT( metaDataComputed( Media* ) ), Qt::DirectConnection );
    emit metadataRequired( media );
}

void
Library::addMedia( Media* media )
{
    QUuid id;
    foreach( id, m_medias.keys() )
    {
        qDebug() << m_medias.value( id )->getFileInfo()->filePath();
        qDebug() << media->getFileInfo()->filePath();
        if ( m_medias.value( id )->getFileInfo()->filePath() == media->getFileInfo()->filePath() )
        {
            deleteMedia( media );
            return;
        }
    }
    m_medias[media->getUuid()] = media;
    metaDataComputed( media );
}

void        Library::addClip( Clip* clip )
{
    m_clips[clip->getUuid()] = clip;
}

void
Library::loadMedia( const QString& path, const QUuid& uuid )
{
    Media*  it;
    foreach ( it, m_medias )
    {
        if ( it->getFileInfo()->absoluteFilePath() == path )
        {
            m_medias.remove( it->getUuid() );
            m_medias[uuid] = it;
            it->setUuid( uuid );
            return ;
        }
    }
    if ( QFile::exists( path ) == false )
    {
        QMessageBox::warning( NULL, tr( "Import error" ), tr( "Can't open file :" ) + path );
        return ;
    }
    Media*  media = new Media( path, uuid );
    connect( media, SIGNAL( metaDataComputed( Media* ) ), this, SLOT( metaDataComputed( Media* ) ) );
    MetaDataManager::getInstance()->computeMediaMetadata( media );
    m_medias[uuid] = media;
}

bool
Library::mediaAlreadyLoaded( const QString& filePath )
{
    Media*   media;
    foreach ( media, m_medias )
    {
        if ( media->getFileInfo()->absoluteFilePath() == filePath )
            return true;
    }
    return false;
}

void
Library::loadProject( const QDomElement& medias )
{
    if ( medias.isNull() == true || medias.tagName() != "medias" )
    {
        qWarning() << "Invalid medias node";
        return ;
    }

    QDomElement elem = medias.firstChild().toElement();
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
            loadMedia( path, uuid );
        }

        elem = elem.nextSibling().toElement();
    }
    emit projectLoaded();
}

void
Library::saveProject( QDomDocument& doc, QDomElement& rootNode )
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

void
Library::clear()
{
    QHash<QUuid, Media*>::iterator  it = m_medias.begin();
    QHash<QUuid, Media*>::iterator  end = m_medias.end();

    while ( it != end )
    {
        emit mediaRemoved( it.key() );
        delete it.value();
        ++it;
    }
    m_medias.clear();
    QHash<QUuid, Clip*>::iterator  it2 = m_clips.begin();
    QHash<QUuid, Clip*>::iterator  end2 = m_clips.end();
    while ( it2 != end2 )
    {
        delete it2.value();
        ++it2;
    }
    m_clips.clear();
}
