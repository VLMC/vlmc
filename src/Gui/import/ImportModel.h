/*****************************************************************************
 * ImportModel.h
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

#ifndef IMPORTMODEL_H
#define IMPORTMODEL_H

#include <QObject>
#include <QHash>
#include <QFileSystemModel>
#include <QDirModel>
#include <QProgressDialog>

#include "Media.h"
#include "Clip.h"
#include "MetaDataManager.h"

class ImportModel : public QObject
{
    Q_OBJECT

public:
    ImportModel();
    ~ImportModel();

    Media*          getMedia( const QUuid& mediaId ) const;
    Clip*           getClip( const QUuid& mediaId, const QUuid& clipId ) const;
    void            cutMedia( const QUuid& mediaId, int frame );
    void            cutClip( const QUuid& mediaId, const QUuid& clipId, int frame );
    void            loadFile( const QFileInfo& fileInfo, int loadingMedias = 1 );
    void            removeMedia( const QUuid& mediaId );
    void            removeClip( const QUuid& mediaId, const QUuid& clipId );
    QHash<QUuid, Media*>*    getMedias() const { return m_medias; }
    void            setFilter( const QStringList& filter ) { m_filters = filter; }
    void            removeAllMedias();

signals:
    void            newMediaLoaded( Media* media );
    void            updateMediaRequested( Media* media );

private:
    QHash<QUuid, Media*>*           m_medias;
    QStringList                     m_filters;
    QList<Media*>                   m_invalidMedias;
    int                             m_loadingMedias;
    int                             m_nbLoadedMedias;
    QProgressDialog*                m_progressDialog;

    void        loadMedia( Media* media );
    bool        mediaAlreadyLoaded( const QFileInfo& fileInfo );

private slots:
    void        metaDataComputed( Media* media );
    void        snapshotComputed( Media* media );
    void        audioSpectrumComputed( Media* media );
};

#endif // IMPORTMODEL_H
