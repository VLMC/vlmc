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
    disconnect( this, SLOT( getMetaData() ) );
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
    disconnect( this, SLOT( renderSnapshot() ) );
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
    QPixmap* pixmap = new QPixmap( m_tmpSnapshotFilename );
    if ( pixmap->isNull() )
        delete pixmap;
    else
        m_currentClip->setSnapshot( pixmap );
    disconnect( this, SLOT( setSnapshot() ) );
    m_mediaPlayer->stop();
}
