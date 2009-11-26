/*****************************************************************************
 * ImportModel.cpp
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Geoffroy Lacarriere <geoffroylaca@gmail.com>
 *          Thomas Boquet <thomas.boquet@gmail.com>
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

#include <QDebug>
#include <QDir>
#include <QMessageBox>

#include "ImportModel.h"

ImportModel::ImportModel()
{
    m_medias = new QHash<QUuid, Media*>();
}

ImportModel::~ImportModel()
{
    QUuid id;

    foreach ( id, m_medias->keys() )
        delete m_medias->value( id );
}

Media*    ImportModel::getMedia( const QUuid& mediaId ) const
{
    return m_medias->value( mediaId );
}

Clip*     ImportModel::getClip( const QUuid& mediaId, const QUuid& clipId ) const
{
    Media* media =  m_medias->value( mediaId );

    if ( !media )
        return NULL;

    return media->clip( clipId );
}

void            ImportModel::cutMedia( const QUuid& mediaId, int frame )
{
    Q_UNUSED( mediaId );
    Q_UNUSED( frame );
}

void            ImportModel::cutClip( const QUuid& mediaId, const QUuid& clipId, int frame )
{
    Q_UNUSED( mediaId );
    Q_UNUSED( clipId );
    Q_UNUSED( frame );
}

void            ImportModel::metaDataComputed( Media* media )
{
    disconnect( media, SIGNAL( metaDataComputed( Media* ) ), this, SLOT( metaDataComputed( Media* ) ) );
    if ( media->getMetadata() == Media::ParsedWithoutSnapshot )
    {
        m_medias->insert( media->getUuid(), media );
        emit newMediaLoaded( media );
        emit updateMediaRequested( media );
    }
    else
        m_invalidMedias.append( media );
    m_nbLoadedMedias++;


    m_progressDialog->setValue( m_nbLoadedMedias );

    if( m_progressDialog->wasCanceled() )
    {
        Media* media;
        foreach( media, m_invalidMedias )
            delete media;
        m_nbLoadedMedias = 0;
        m_invalidMedias.clear();
        return;
    }

    if ( m_nbLoadedMedias == m_loadingMedias )
    {
        if ( m_invalidMedias.count() > 0 )
        {
            QStringList list;
            Media* media;
            foreach( media, m_invalidMedias )
            {
                list.append( media->getFileName() );
                delete media;
            }
            QMessageBox::warning( NULL, QString( "Error!" ), QString( tr( "Error while loading media(s):\n%0" ) ).arg( list.join( QString("\n") ) ) );
            m_invalidMedias.clear();
        }
        m_nbLoadedMedias = 0;
    }
}

void            ImportModel::snapshotComputed( Media *media )
{
    disconnect( media, SIGNAL( snapshotComputed( Media* ) ), this, SLOT( snapshotComputed( Media* ) ) );
    emit updateMediaRequested( media );
}

void            ImportModel::loadMedia( Media* media )
{
    connect( media, SIGNAL( metaDataComputed( Media* ) ), this, SLOT( metaDataComputed( Media* ) ) );
    connect( media, SIGNAL( snapshotComputed(Media*) ), this, SLOT( snapshotComputed(Media*) ) );
    MetaDataManager::getInstance()->computeMediaMetadata( media );
}

bool        ImportModel::mediaAlreadyLoaded( const QFileInfo& fileInfo )
{
    QUuid id;
    foreach(id, m_medias->keys())
        if ( m_medias->value( id )->getFileInfo()->filePath() == fileInfo.filePath() )
            return true;
    return false;
}

void            ImportModel::loadFile( const QFileInfo& fileInfo, int loadingMedias )
{
    Media* media;

    if ( !fileInfo.isDir() )
    {
        if ( loadingMedias == 0)
        {
            m_loadingMedias = 1;
            m_progressDialog = new QProgressDialog("Importing files...", "Cancel", 0, m_loadingMedias, NULL);
            m_progressDialog->setWindowModality( Qt::WindowModal );
            m_progressDialog->setMinimumDuration( 1000 );
            m_progressDialog->setValue( 0 );
            m_nbLoadedMedias = 0;
        }
        if ( !mediaAlreadyLoaded( fileInfo ) )
        {
           media = new Media( fileInfo.filePath() );
           loadMedia( media );
        }
        else
            m_nbLoadedMedias++;
    }
    else
    {
        QDir dir = QDir( fileInfo.filePath() );
        QFileInfoList list = dir.entryInfoList( m_filters );
        QFileInfo file;

        m_loadingMedias = list.count();
        m_nbLoadedMedias = 0;
        m_progressDialog = new QProgressDialog("Importing files...", "Cancel", 0, m_loadingMedias, NULL);
        m_progressDialog->setWindowModality(Qt::WindowModal);
        m_progressDialog->setMinimumDuration( 1000 );
        m_progressDialog->setValue( 0 );
        foreach( file, list )
            loadFile( file, m_loadingMedias );
    }
}

void            ImportModel::removeMedia( const QUuid& mediaId)
{
    m_medias->remove( mediaId );
}

void            ImportModel::removeClip( const QUuid& mediaId, const QUuid& clipId )
{
    if ( ! m_medias->contains( mediaId ) )
        return ;

    m_medias->value( mediaId )->removeClip( clipId );
}
