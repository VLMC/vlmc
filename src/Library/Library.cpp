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
    m_progressDialog = new QProgressDialog( tr("Importing files..."),
                                            tr("Cancel"), 0, m_loadingMedias, NULL);
    m_progressDialog->setWindowModality( Qt::WindowModal );
    m_progressDialog->setMinimumDuration( 1000 );
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
    Media* media = m_medias.value( mediaUuid );
    if ( media == NULL )
        return NULL;
    return dynamic_cast<Clip*>( getElementByUuid( *media->clips(), clipUuid ) );
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
    if ( m_medias.contains( uuid ) &&
         m_medias.value( uuid )->getMetadata() == Media::ParsedWithAudioSpectrum )
    {
        disconnect( m_medias.value( uuid ),
                    SIGNAL( audioSpectrumComputed( const QUuid& ) ),
                    this,
                    SLOT( deleteMedia( const QUuid& ) ) );
        delete m_medias.take( uuid );
    }
    else if ( m_temporaryMedias.contains( uuid ) &&
              m_temporaryMedias.value( uuid )->getMetadata() ==
              Media::ParsedWithAudioSpectrum )
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
Library::addMedia( const QFileInfo& fileInfo )
{
    Media* media = new Media( fileInfo.filePath() );
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

    m_progressDialog->setValue(m_nbLoadedMedias);
    emit newMediaLoaded( media->getUuid() );
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
            m_progressDialog->setMaximum( 1 );
            m_progressDialog->setValue( 0 );
            m_loadingMedias = 1;
            m_nbLoadedMedias = 0;
        }

        if ( !mediaAlreadyLoaded( fileInfo ) )
            addMedia( fileInfo );
        else
        {
            m_progressDialog->setMaximum( m_loadingMedias-- );
            m_progressDialog->setValue( ++m_nbLoadedMedias );
        }
    }
    else
    {
        QDir dir = QDir( fileInfo.filePath() );
        QFileInfoList list = dir.entryInfoList( m_filters );
        QFileInfo file;

        m_loadingMedias = list.count();
        m_nbLoadedMedias = 0;
        m_progressDialog->setMaximum( m_loadingMedias );
        m_progressDialog->setValue( 0 );
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

        QDomCharacterData text;
        text = doc.createTextNode( it.value()->getFileInfo()->absoluteFilePath() );

        QDomElement uuid = doc.createElement( "uuid" );
        QDomCharacterData text2 = doc.createTextNode( it.value()->getUuid().toString() );

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
    qDebug() << "removing clip" << clipId << "in media" << mediaId;
    Media*  med = 0;
    if ( m_medias.contains( mediaId ) )
        med = m_medias[mediaId];
    else
        return;

    qDebug() << "clips :" << *med->clips();
    if ( med->clips()->contains( clipId ) )
    {
        qDebug() << "clips before :" << med->clips()->size();
        med->removeClip( clipId );
        qDebug() << "clips :" << med->clips()->size();
    }
}
