#ifndef METADATAMANAGER_H
#define METADATAMANAGER_H

#include <QList>
#include <QTemporaryFile>
#include <QThread>
#include "gui/LibraryWidget.h"
#include "VLCMediaPlayer.h"

class MetaDataManager : public QThread
{
    Q_OBJECT
    Q_DISABLE_COPY( MetaDataManager )

    public:
        MetaDataManager( LibraryWidget* libraryWidget );
        ~MetaDataManager();

    private:
        virtual void    run();

    private:
        LibraryWidget*              m_libraryWidget;
        LibVLCpp::MediaPlayer*      m_mediaPlayer;
        QWidget*                    m_renderWidget;

        // TODO: THREAD SAFING
        QList<ListViewMediaItem*>   m_mediaList;

        // Thread component
        bool                        m_nextMedia;
        ListViewMediaItem*          m_currentMediaItem;
        char*                       m_tmpSnapshotFilename;

    private slots:
        void    listViewMediaAdded( ListViewMediaItem* );
        void    renderSnapshot();
        void    setSnapshotInIcon();
};

#endif // METADATAMANAGER_H
