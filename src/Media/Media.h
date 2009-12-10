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
#include <QHash>

#include "VLCMedia.h"
#include "Clip.h"

class Clip;

/**
  * Represents a basic container for media informations.
  */
class       Media : public QObject
{
    Q_OBJECT

public:
    /**
     *  \enum fType
     *  \brief enum to determine file type
     */
    enum    FileType
    {
        Audio,
        Video,
        Image
    };
    enum    InputType
    {
        File,
        Stream
    };
    enum    MetadataState
    {
        None,
        ParsedWithoutSnapshot,
        ParsedWithSnapshot,
        ParsedWithAudioSpectrum
    };
    Media( const QString& filePath, const QString& uuid = QString() );
    virtual ~Media();

    /**
     *  \brief  This method adds a parameter that will stay constant though the whole life of this media (unless it is explicitely overided), even if it is cloned.
     */
    void                        addConstantParam( const QString& param );
    /**
     *  \brief  This method will add a parameter that will be restored to defaultValue when the flushVolatileParameter is called
     */
    void                        addVolatileParam( const QString& param, const QString& defaultValue );
    void                        flushVolatileParameters();
    LibVLCpp::Media*            getVLCMedia() { return m_vlcMedia; }

    void                        setSnapshot( QPixmap* snapshot );
    const QPixmap&              getSnapshot() const;

    const QFileInfo*            getFileInfo() const;
    const QString&              getMrl() const;
    const QString&              getFileName() const;

    /**
        \return                 Returns the length of this media (ie the
                                video duration) in milliseconds.
    */
    qint64                      getLengthMS() const;
    /**
        \brief                  This methods is most of an entry point for the
                                MetadataManager than enything else.
                                If you use it to set a inconsistant media length
                                you'll just have to blame yourself !
    */
    void                        setLength( qint64 length );
    void                        setNbFrames( qint64 nbFrames );

    int                         getWidth() const;
    void                        setWidth( int width );

    int                         getHeight() const;
    void                        setHeight( int height );

    float                       getFps() const;
    void                        setFps( float fps );

    qint64                      getNbFrames() const;

    const QUuid&                getUuid() const;
    void                        setUuid( const QUuid& uuid );

    FileType                    getFileType() const;
    static const QString        VideoExtensions;
    static const QString        AudioExtensions;
    static const QString        ImageExtensions;
    InputType                   getInputType() const;
    static const QString        streamPrefix;

    const QStringList&          getMetaTags() const;
    void                        setMetaTags( const QStringList& tags );
    bool                        matchMetaTag( const QString& tag ) const;

    void                        emitMetaDataComputed( bool hasMetadata );
    void                        emitSnapshotComputed();
    void                        emitAudioSpectrumComuted();

//    bool                        hasMetadata() const;
    MetadataState               getMetadata() const;

    void                        addClip( Clip* clip );
    void                        removeClip( const QUuid& uuid );
    Clip*                       clip( const QUuid& uuid ) const { return m_clips[uuid]; }
    const QHash<QUuid, Clip*>*  clips() const { return &m_clips; }

    QList<int>*                 getAudioValues() { return m_audioValueList; }


private:
    void                        setFileType();

protected:
    static QPixmap*             defaultSnapshot;
    LibVLCpp::Media*            m_vlcMedia;
    QString                     m_mrl;
    QList<QString>              m_volatileParameters;
    QPixmap*                    m_snapshot;
    QUuid                       m_uuid;
    QFileInfo*                  m_fileInfo;
    qint64                      m_lengthMS;
    qint64                      m_nbFrames;
    unsigned int                m_width;
    unsigned int                m_height;
    float                       m_fps;
    FileType                    m_fileType;
    InputType                   m_inputType;
    MetadataState               m_metadataState;
    QString                     m_fileName;
    QStringList                 m_metaTags;
    QHash<QUuid, Clip*>         m_clips;
    QList<int>*                 m_audioValueList;

signals:
    void                        metaDataComputed( Media* );
    void                        snapshotComputed( Media* );
    void                        audioSpectrumComputed( Media* );
};

#endif // CLIP_H__
