#include <QDebug>
#include <QDir>

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

const Media*    ImportModel::getMedia( const QUuid& mediaId ) const
{
    return m_medias->value( mediaId );
}

const Clip*     ImportModel::getClip( const QUuid& mediaId, const QUuid& clipId ) const
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
    qDebug() << "Meta computed";
    emit newMediaLoaded( media );
}

void            ImportModel::loadMedia( Media* media )
{
    if ( !m_medias->contains( media->getUuid() ) )
    {
        m_medias->insert( media->getUuid(), media );
        //emit mediaAdded( media, m_mediaList->getCell( media->getUuid() ) );

        connect( media, SIGNAL( metaDataComputed( Media* ) ), this, SLOT( metaDataComputed( Media* ) ) );
        m_metaDataWorker = new MetaDataWorker( media );
        m_metaDataWorker->start();
    }
    else
        delete media;
}

void            ImportModel::loadFile( const QFileInfo& fileInfo )
{
    Media* media;

    if ( !fileInfo.isDir() )
    {
        media = new Media( fileInfo.filePath() );
        loadMedia( media );
    }
    else
    {
        QDir dir = QDir( fileInfo.filePath() );
        for( int i = 0; i < dir.count() ; i++)
        {
            QFileInfo info = QFileInfo(dir.filePath( dir[i] ) );
            //qDebug() << info.filePath();
            if ( info.isDir() )
                continue ;
            //qDebug() << "not a dir [" << info.fileName() << "]";
            media = new Media( info.filePath() );
            loadMedia( media );
        }
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
