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

struct          audioData
{
    void*               datas;
    unsigned int*       freq;
    unsigned int*       nbChannels;
    unsigned int*       fourCCFormat;
    unsigned int*       frameSize;
    unsigned int        nbSample;
    unsigned char*      buffer;
    size_t              buffSize;
    QVector<int*>       frameList;
};

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
    Media( const QString& filePath, const QString& = QString() );
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
    qint64                      getLength() const;
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

    void                        initAudioData( void* datas, unsigned int* freq, unsigned int* nbChannels, unsigned int* fourCCFormat, unsigned int* frameSize );
    void                        addAudioFrame( void* datas, unsigned char* buffer, size_t buffSize, unsigned int nbSample );

    audioData*                  getAudioData() { return &m_audioData; }
    QVector<int*>               getAudioFrameList() { return m_audioData.frameList; }
    unsigned int                getAudioNbSample() { return m_audioData.nbSample; }

    void                        emitMetaDataComputed();

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
    qint64                      m_length;
    qint64                      m_nbFrames;
    unsigned int                m_width;
    unsigned int                m_height;
    float                       m_fps;
    int*                        m_audioSpectrum;
    audioData                   m_audioData;
    FileType                    m_fileType;
    InputType                   m_inputType;
    QString                     m_fileName;

signals:
    void                        metaDataComputed();
    void                        metaDataComputed( Media* );
};

#endif // CLIP_H__
