#ifndef IMPORTMODEL_H
#define IMPORTMODEL_H

#include <QObject>
#include <QHash>

#include "Media.h"
#include "Clip.h"
#include "MetaDataWorker.h"

class ImportModel : public QObject
{
    Q_OBJECT

public:
    ImportModel();
    ~ImportModel();

    const Media*    getMedia( const QUuid& mediaId ) const;
    const QHash<QUuid, Media*>*    getMedias() const;
    const Clip*     getClip( const QUuid& mediaId, const QUuid& clipId ) const;
    void            cutMedia( const QUuid& mediaId, int frame );
    void            cutClip( const QUuid& mediaId, const QUuid& clipId, int frame );
    void            loadFile( const QFileInfo& fileInfo );
    void            removeMedia( const QUuid& mediaId );
    void            removeClip( const QUuid& mediaId, const QUuid& clipId );

signals:
    void            newMediaLoaded( Media* media );
    void            updateMediaRequested( Media* media );

private:
    QHash<QUuid, Media*>*           m_medias;
    MetaDataWorker*                 m_metaDataWorker;

    void        loadMedia( Media* media );

private slots:
    void        metaDataComputed( Media* media );
};

#endif // IMPORTMODEL_H
