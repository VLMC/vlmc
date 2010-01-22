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
  * It can load and unload Clips (Clip.h/Clip.cpp)
  */

#include "Clip.h"
#include "Library.h"
#include "Media.h"
#include "MetaDataManager.h"

#include <QDebug>
#include <QDir>
#include <QDomElement>
#include <QHash>
#include <QMessageBox>
#include <QProgressDialog>
#include <QUuid>

Library::Library()
{
    m_nbLoadedMedias = 0;
}

Media*
Library::media( const QUuid& uuid )
{
    return getElementByUuid( m_medias, uuid );
}

Media*
Library::temporaryMedia( const QUuid& uuid )
{
    return getElementByUuid( m_temporaryMedias, uuid );
}

Clip*
Library::clip( const QUuid& uuid )
{
    if ( m_medias.contains( uuid ) )
    {
        Media* media = m_medias.value( uuid );
        if ( media->baseClip() == NULL )
            return NULL;
        Clip* clip = new Clip( media->baseClip() );
        return clip;
    }

    QUuid mediaUuid;
    foreach( mediaUuid, m_medias.keys() )
    {
        Media* media = m_medias.value( mediaUuid );
        if ( media != NULL && media->clips()->contains( uuid ) )
            return dynamic_cast<Clip*>( getElementByUuid( *media->clips(), uuid ) );
    }
    return NULL;
}

Clip*
Library::clip( const QUuid& mediaUuid, const QUuid& clipUuid )
{
    if ( m_medias.contains( mediaUuid ) )
    {
        if ( m_medias.value( mediaUuid )->clips()->contains( clipUuid ) )
            return m_medias.value( mediaUuid )->clip( clipUuid );
        else
            return new Clip( m_medias.value( mediaUuid )->baseClip() );
    }
    return NULL;
}

void
Library::removingMediaAsked( const QUuid& uuid )
{
    deleteMedia( uuid );
    emit mediaRemoved( uuid );
}

void
Library::deleteMedia( const QUuid& uuid )
{
    if ( m_medias.contains( uuid ) )
    {
        disconnect( m_medias.value( uuid ),
                    SIGNAL( audioSpectrumComputed( const QUuid& ) ),
                    this,
                    SLOT( deleteMedia( const QUuid& ) ) );
        delete m_medias.take( uuid );
    }
    else if ( m_temporaryMedias.contains( uuid ) )
    {
        disconnect( m_medias.value( uuid ),
                    SIGNAL( audioSpectrumComputed( const QUuid& ) ),
                    this,
                    SLOT( deleteMedia( const QUuid& ) ) );
        delete m_temporaryMedias.take( uuid );
    }
    else if ( m_mediaToDelete.contains( uuid ) )
    {
        delete m_mediaToDelete.value( uuid );
    }
    else if ( m_medias.contains( uuid ) )
    {
        Media* media = m_medias.take( uuid );
        m_mediaToDelete.insert( uuid, media );
        connect( media,
                 SIGNAL( audioSpectrumComputed( const QUuid& ) ),
                 this,
                 SLOT( deleteMedia( const QUuid& ) ) );
    }
    else if ( m_temporaryMedias.contains( uuid ) )
    {
        Media* media = m_temporaryMedias.take( uuid );
        m_mediaToDelete.insert( uuid, media );
        connect( media,
                 SIGNAL( audioSpectrumComputed( const QUuid& ) ),
                 this,
                 SLOT( deleteMedia( const QUuid& ) ) );
    }
}

void
Library::addMedia( const QFileInfo& fileInfo, const QString& uuidStr, bool emitSignal )
{
    Media* media = new Media( fileInfo.filePath(), uuidStr );
    m_nbLoadedMedias++;

    QUuid id;
    foreach( id, m_medias.keys() )
        if ( m_medias.value( id )->getFileInfo()->filePath() ==
             media->getFileInfo()->filePath() )
        {
            delete media;
            return;
        }

    connect( media,
             SIGNAL( metaDataComputed( Media* ) ),
             this,
             SLOT( metaDataComputed( Media* ) ), Qt::QueuedConnection );
    connect( media,
             SIGNAL( snapshotComputed( Media* ) ),
             this,
             SLOT( snapshotComputed( Media* ) ), Qt::QueuedConnection );

    MetaDataManager::getInstance()->computeMediaMetadata( media );

    m_temporaryMedias[media->getUuid()] = media;

    if ( emitSignal )
    {
        emit progressDialogValue( m_nbLoadedMedias );
        emit newMediaLoaded( media->getUuid() );
    }
}

void
Library::addClip( Clip* clip )
{
    Media* media = m_medias[clip->getParent()->getUuid()];
    media->addClip( clip );
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
        QMessageBox::warning( NULL, tr( "Import error" ),
                              tr( "Can't open file :" ) + path );
        return ;
    }
    Media*  media = new Media( path, uuid );
    connect( media,
             SIGNAL( metaDataComputed( Media* ) ),
             this,
             SLOT( metaDataComputed( Media* ) ) );
    MetaDataManager::getInstance()->computeMediaMetadata( media );
    m_medias[uuid] = media;
}

void
Library::metaDataComputed( Media* media )
{
    disconnect( media,
                SIGNAL( metaDataComputed( Media* ) ),
                this,
                SLOT( metaDataComputed( Media* ) ) );
}

void
Library::snapshotComputed( Media *media )
{
    disconnect( media,
                SIGNAL( snapshotComputed( Media* ) ),
                this,
                SLOT( snapshotComputed( Media* ) ) );
    emit updateMediaRequested( media->getUuid() );
}

bool
Library::mediaAlreadyLoaded( const QString& filePath )
{
    QUuid id;
    foreach( id, m_temporaryMedias.keys() )
    {
        Media* media = m_temporaryMedias.value( id );
        if ( media->getFileInfo()->filePath() == filePath )
            return true;
    }
    foreach( id, m_medias.keys() )
    {
        Media* media = m_medias.value( id );
        if ( media->getFileInfo()->filePath() == filePath )
            return true;
    }
    return false;
}

bool
Library::mediaAlreadyLoaded( const QFileInfo& fileInfo )
{
    QUuid id;
    foreach( id, m_temporaryMedias.keys() )
    {
        Media* media = m_temporaryMedias.value( id );
        if ( media->getFileInfo()->filePath() == fileInfo.filePath() )
            return true;
    }
    foreach( id, m_medias.keys() )
    {
        Media* media = m_medias.value( id );
        if ( media->getFileInfo()->filePath() == fileInfo.filePath() )
            return true;
    }
    return false;
}

void
Library::loadFile( const QFileInfo& fileInfo, int loadingMedias )
{
    if ( !fileInfo.isDir() )
    {
        if ( loadingMedias == 1 )
        {
            emit progressDialogMax( 1 );
            emit progressDialogValue( 0 );
            m_loadingMedias = 1;
            m_nbLoadedMedias = 0;
        }

        if ( !mediaAlreadyLoaded( fileInfo ) )
            addMedia( fileInfo );
        else
        {
            emit progressDialogMax( m_loadingMedias-- );
            emit progressDialogValue( ++m_nbLoadedMedias );
        }
    }
    else
    {
        QDir dir = QDir( fileInfo.filePath() );
        QFileInfoList list = dir.entryInfoList( m_filters );
        QFileInfo file;

        m_loadingMedias = list.count();
        m_nbLoadedMedias = 0;
        emit progressDialogMax( m_loadingMedias );
        emit progressDialogValue( 0 );
        foreach( file, list )
            loadFile( file, m_loadingMedias );
    }
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
        QList<QDomElement>  clipList;
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
            else if ( tagName == "clips" )
            {
                QDomElement clip = mediaProperty.firstChild().toElement();
                while ( clip.isNull() == false )
                {
                    clipList.push_back( clip );
                    clip = clip.nextSibling().toElement();
                }
            }
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
            addMedia( path, uuid, false );
            //FIXME : this is ugly
            importDone();
        }
        if ( clipList.size() != 0 )
        {
            foreach( QDomElement clip, clipList )
            {
                QString parentUuid = clip.attribute( "parentUuid", "");
                if ( parentUuid != "" && parentUuid == uuid )
                {
                    QString beg = clip.attribute( "begin", "" );
                    QString end = clip.attribute( "end", "" );
                    QString clipUuid = clip.attribute( "uuid", "" );
                    if ( beg != "" && end != "" && uuid != "" )
                    {
                        Media* media = m_medias[QUuid( uuid )];
                        if ( media != 0 )
                        {
                            Clip* clip = new Clip( media, beg.toInt(), end.toInt(), QUuid( clipUuid ) );
                            media->addClip( clip );
                        }
                    }
                }
            }
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

        QDomCharacterData text;
        text = doc.createTextNode( it.value()->getFileInfo()->absoluteFilePath() );

        QDomElement uuid = doc.createElement( "uuid" );
        QDomCharacterData text2 = doc.createTextNode( it.value()->getUuid().toString() );

        mrl.appendChild( text );
        uuid.appendChild( text2 );

        media.appendChild( mrl );
        media.appendChild( uuid );
        //Creating the clip branch
        if ( it.value()->clips()->size() != 0 )
        {
            QDomElement clips = doc.createElement( "clips" );
            foreach( Clip* c, it.value()->clips()->values() )
            {
                QDomElement clip = doc.createElement( "clip" );
                clip.setAttribute( "begin", c->getBegin() );
                clip.setAttribute( "end", c->getEnd() );
                clip.setAttribute( "uuid", c->getUuid() );
                clip.setAttribute( "parentUuid", c->getParent()->getUuid() );
                clips.appendChild( clip );
            }
            media.appendChild( clips );
        }
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
}

void
Library::deleteTemporaryMedias()
{
    QUuid uuid;
    foreach( uuid, m_temporaryMedias.keys() )
        deleteMedia( uuid );
}

void
Library::importDone()
{
    QUuid id;
    foreach( id, m_temporaryMedias.keys() )
    {
        Media* media = m_temporaryMedias.value( id );
        m_medias.insert( media->getUuid(), media );
        emit newMediaImported( media->getUuid() );
    }
    m_temporaryMedias.clear();
}

void
Library::removeClip( const QUuid& mediaId, const QUuid& clipId )
{
    Media*  med = 0;
    if ( m_medias.contains( mediaId ) )
        med = m_medias[mediaId];
    else
        return;

    if ( med->clips()->contains( clipId ) )
        med->removeClip( clipId );
}
