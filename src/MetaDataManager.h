#ifndef METADATAMANAGER_H
#define METADATAMANAGER_H

#include <QList>
#include <QTemporaryFile>
#include <QThread>
#include <QWidget>
#include "Clip.h"
#include "VLCMediaPlayer.h"

class MetaDataManager : public QThread
{
    Q_OBJECT
    Q_DISABLE_COPY( MetaDataManager )

    public:
        MetaDataManager();
        ~MetaDataManager();

    private:
        virtual void                run();

    private:
        LibVLCpp::MediaPlayer*      m_mediaPlayer;
        QWidget*                    m_renderWidget;

        // TODO: THREAD SAFING
        QList<Clip*>   m_mediaList;

        // Thread component
        bool                        m_nextMedia;
        Clip*                       m_currentClip;
        //FIXME: Won't work in asynchrone mode
        char*                       m_tmpSnapshotFilename;

    private slots:
        void    renderSnapshot();
        void    getMetaData();
        void    newClipLoaded( Clip* );
        void    setSnapshot();
};

#endif // METADATAMANAGER_H
