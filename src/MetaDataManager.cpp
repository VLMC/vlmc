/** \file
  * This file contains the MetaDataManager class implementation.
  * It used by the library in Library.[hc]pp.
  * It parse a Clip (see in Clip.[hc]pp) to get meta-data like length, first image, etc.
  */

#include <QtDebug>
#include "MetaDataManager.h"
#include "Library.h"


MetaDataManager::MetaDataManager() : m_renderWidget( NULL )
{
    m_mediaPlayer = new LibVLCpp::MediaPlayer();
    connect( Library::getInstance(), SIGNAL( newClipLoaded( Clip* ) ),this, SLOT( newClipLoaded( Clip* ) ) );
    m_tmpSnapshotFilename = new char[512];
    m_renderWidget = new QWidget();
    m_mediaPlayer->setDrawable( m_renderWidget->winId() );
}

MetaDataManager::~MetaDataManager()
{
    if (m_mediaPlayer)
        delete m_mediaPlayer;
    if (m_renderWidget)
        delete m_renderWidget;
    delete[] m_tmpSnapshotFilename;
}

void    MetaDataManager::newClipLoaded( Clip* item )
{
    m_mediaList.append( item );

    if ( !isRunning() )
        start();
}

void    MetaDataManager::run()
{
    m_nextMedia = true;
    while ( !m_mediaList.isEmpty() )
    {
        if ( m_nextMedia )
        {
            qDebug() << "Laucnhing thread for a new video";
            m_nextMedia = false;

            m_currentClip = m_mediaList.front();
            m_mediaList.pop_front();

            //Disabling audio for this specific use of the media
            m_currentClip->addParam( ":no-audio" );
            m_currentClip->flushParameters();
            //And re-enable it to prevent the audio to be disabled anywhere else.
            m_currentClip->addParam( ":audio" );

            m_mediaPlayer->setMedia( m_currentClip->getVLCMedia() );
            m_mediaPlayer->play();

            connect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( getMetaData() ) );
        }
        usleep( 10000 );
    }
    return;
}

void    MetaDataManager::getMetaData()
{
    qDebug() << "getMetaData";
    disconnect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( getMetaData() ) );

    m_nextMedia = true;
    m_currentClip->setLength( m_mediaPlayer->getLength() );
    m_currentClip->setWidth( m_mediaPlayer->getWidth() );
    m_currentClip->setHeight( m_mediaPlayer->getHeight() );

    //Setting time for snapshot :
    connect( m_mediaPlayer, SIGNAL( positionChanged() ), this, SLOT( renderSnapshot() ) );
    m_mediaPlayer->setTime( m_mediaPlayer->getLength() / 3 );
}

void    MetaDataManager::renderSnapshot()
{
    qDebug() << "Rendering snapshot";
    disconnect( m_mediaPlayer, SIGNAL( positionChanged() ), this, SLOT( renderSnapshot() ) );
    QTemporaryFile tmp;
    tmp.setAutoRemove( false );
    tmp.open();

    strncpy(m_tmpSnapshotFilename, tmp.fileName().toStdString().c_str(), tmp.fileName().length());

    connect( m_mediaPlayer, SIGNAL( snapshotTaken() ), this, SLOT( setSnapshot() ) );

    //The slot should be triggered in this methode
    m_mediaPlayer->takeSnapshot( m_tmpSnapshotFilename, 32, 32 );
    //Snapshot slot should has been called (but maybe not in next version...)
}

void    MetaDataManager::setSnapshot()
{
    qDebug() << "Setting snapshot";
    QPixmap* pixmap = new QPixmap( m_tmpSnapshotFilename );
    if ( pixmap->isNull() )
        delete pixmap;
    else
        m_currentClip->setSnapshot( pixmap );
    disconnect( m_mediaPlayer, SIGNAL( snapshotTaken() ), this, SLOT( setSnapshot() ) );

    //CHECKME:
    //This is synchrone, but it may become asynchrone in the future...

    //connect( m_mediaPlayer, SIGNAL( stopped() ), this, SLOT( startAudioDataParsing() ) );
    qDebug() << "Stopping playback";
    m_mediaPlayer->stop();
    //startAudioDataParsing();
}

void    MetaDataManager::startAudioDataParsing()
{
    qDebug() << "Starting audio parsing";
    char    osb[64], psb[64], csb[64], iph[64], data[64];

    disconnect( m_mediaPlayer, SIGNAL( stopped() ), this, SLOT( startAudioDataParsing() ) );

    //Deactivating video, so that real time doesn't matter
    sprintf( osb, ":amem-opensb=%lld", (long long int)(intptr_t) &MetaDataManager::openSoundBuffer);
    sprintf( psb, ":amem-playsb=%lld", (long long int)(intptr_t) &MetaDataManager::playSoundBuffer);
    sprintf( csb, ":amem-closesb=%lld", (long long int)(intptr_t) &MetaDataManager::closeSoundBuffer);
    sprintf( iph, ":amem-iph=%lld", (long long int)(intptr_t) &MetaDataManager::instanceParameterHandler);
    sprintf( data, ":amem-data=%lld", (long long int)(intptr_t) this);
//    m_currentClip->addParam( ":no-video" );
//    m_currentClip->addParam( ":audio" );
    m_currentClip->addParam( ":aout=amem" );
    m_currentClip->addParam( osb );
    m_currentClip->addParam( psb );
    m_currentClip->addParam( csb );
    m_currentClip->addParam( iph );
    m_currentClip->addParam( data );
    m_currentClip->flushParameters();

    m_mediaPlayer->setMedia( m_currentClip->getVLCMedia() );
    qDebug() << "Starting playback again";
    m_mediaPlayer->play();

    //Restoring the clip at a correct value.
//    m_currentClip->addParam( ":video" );
}

void    MetaDataManager::openSoundBuffer( void* datas, unsigned int* freq, unsigned int* nbChannels, unsigned int* fourCCFormat, unsigned int* frameSize )
{
    qDebug() << "Opening sound buffer with freq =" << freq;
}

void    MetaDataManager::playSoundBuffer( void* datas, unsigned char* buffer, size_t buffSize, unsigned int nbSample )
{
//    qDebug() << "Playing sound buffer with nbSample =" << nbSample;
}

void    MetaDataManager::closeSoundBuffer( void* datas )
{
    qDebug() << "Closing sound buffer";
}

void    MetaDataManager::instanceParameterHandler( void*, char*, char* )
{
}
