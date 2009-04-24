/*****************************************************************************
 * Media.h : Represents a basic container for media informations.
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
  * This file contains the Media class declaration/definition.
  * It contains a VLCMedia and the meta-datas.
  * It's used by the Library
  */

#ifndef MEDIA_H__
#define MEDIA_H__

#include <QList>
#include <QString>
#include <QPixmap>
#include <QUuid>
#include <QObject>
#include <QFileInfo>

#include "VLCMedia.h"

/**
  * Represents a basic container for media informations.
  */
class       Media : public QObject
{
    Q_OBJECT

public:
    Media( const QFileInfo* fileInfo );
    Media( const QString& mrl );
    virtual ~Media();

    void                        loadMedia( const QString& mrl );
    void                        addParam( const QString& param );
    void                        flushParameters();
    LibVLCpp::Media*            getVLCMedia() { return m_vlcMedia; }

    void                        setSnapshot( QPixmap* snapshot );
    const QPixmap&              getSnapshot() const;

    const QFileInfo*            getFileInfo() const;

    qint64                      getLength() const;
    void                        setLength( qint64 length );

    int                         getWidth() const;
    void                        setWidth( int width );

    int                         getHeight() const;
    void                        setHeight( int height );

    const QUuid&                getUuid() const;

protected:
    static QPixmap*             defaultSnapshot;

    LibVLCpp::Media*            m_vlcMedia;
    QString                     m_mrl;
    QList<QString>              m_parameters;
    QPixmap*                    m_snapshot;
    QUuid                       m_uuid;
    QFileInfo*                  m_fileInfo;
    qint64                      m_length;
    unsigned int                m_width;
    unsigned int                m_height;

signals:
    void                        snapshotChanged();
};

#endif // CLIP_H__
