/** \file
  * This file contains the MetaDataManager class declaration/definition.
  * It used by the library in Library.[hc]pp.
  * It parse a Clip (see in Clip.[hc]pp) to get meta-data like length, first image, etc.
  */

#ifndef METADATAMANAGER_H
#define METADATAMANAGER_H

#include <QList>
#include <QTemporaryFile>
#include <QThread>
#include <QWidget>
#include "Media.h"
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
        //AMEM part :
        static  void                openSoundBuffer( void* datas, unsigned int* freq,
                                                        unsigned int* nbChannels, unsigned int* fourCCFormat,
                                                        unsigned int* frameSize );
        static  void                playSoundBuffer( void* datas, unsigned char* buffer,
                                                     size_t buffSize, unsigned int nbSample );
        static  void                closeSoundBuffer( void* datas );
        static  void                instanceParameterHandler( void*, char*, char* );

    private:
        LibVLCpp::MediaPlayer*      m_mediaPlayer;
        QWidget*                    m_renderWidget;

        // TODO: THREAD SAFING
        QList<Media*>               m_mediaList;

        // Thread component
        bool                        m_nextMedia;
        Media*                      m_currentClip;
        //FIXME: Won't work in asynchrone mode
        char*                       m_tmpSnapshotFilename;

    private slots:
        void    renderSnapshot();
        void    getMetaData();
        void    newMediaLoaded( Media* );
        void    setSnapshot();
        void    startAudioDataParsing();
};

#endif // METADATAMANAGER_H
