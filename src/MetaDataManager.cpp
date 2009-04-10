#include "MetaDataManager.h"

MetaDataManager::MetaDataManager( LibraryWidget* libraryWidget ) : m_libraryWidget( libraryWidget ), m_renderWidget( NULL )
{
    m_mediaPlayer = new LibVLCpp::MediaPlayer();
    connect( m_libraryWidget, SIGNAL( listViewMediaAdded( ListViewMediaItem* ) ), this, SLOT( listViewMediaAdded( ListViewMediaItem* ) ) );
    m_tmpSnapshotFilename = new char[512];
    m_renderWidget = new QWidget();
}

MetaDataManager::~MetaDataManager()
{
    if (m_mediaPlayer)
        delete m_mediaPlayer;
    if (m_renderWidget)
        delete m_renderWidget;
    delete[] m_tmpSnapshotFilename;
}

void    MetaDataManager::listViewMediaAdded( ListViewMediaItem* item )
{
    m_mediaList.append( item );

    if ( !isRunning() )
        start();
}

void    MetaDataManager::run()
{
    m_nextMedia = true;

//    if ( !m_renderWidget )

        m_mediaPlayer->setDrawable( m_renderWidget->winId() );

    while ( !m_mediaList.isEmpty() )
    {
        if ( m_nextMedia )
        {
            m_nextMedia = false;

            m_currentMediaItem = m_mediaList.front();
            m_mediaList.pop_front();
            m_currentMediaItem->getInputMedia()->setMediaPlayer( m_mediaPlayer );

            m_mediaPlayer->setMedia( m_currentMediaItem->getInputMedia()->getVLCMedia() );

            connect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( renderSnapshot() ) );

            m_mediaPlayer->play();
        }
        usleep( 100 );
    }
    return;
}

void    MetaDataManager::renderSnapshot()
{
    m_currentMediaItem->getInputMedia()->setTime( m_currentMediaItem->getInputMedia()->getLength() / 3 );

    QTemporaryFile tmp;
    tmp.setAutoRemove( false );
    tmp.open();

    strncpy(m_tmpSnapshotFilename, tmp.fileName().toStdString().c_str(), tmp.fileName().length());

    connect( m_mediaPlayer, SIGNAL( snapshotTaken() ), this, SLOT( setSnapshotInIcon() ) );

    sleep(1);
    //The slot should be triggered in this methode
    m_mediaPlayer->takeSnapshot( m_tmpSnapshotFilename, 32, 32 );
    //Snapshot slot should has been called (but maybe not in next version...)
}

void    MetaDataManager::setSnapshotInIcon()
{
    m_currentMediaItem->getInputMedia()->setSnapshot( new QPixmap( m_tmpSnapshotFilename ) );
    m_currentMediaItem->setIcon( QIcon( m_currentMediaItem->getInputMedia()->getSnapshot() ) );
    m_nextMedia = true;
    disconnect( this, SLOT( setSnapshotInIcon() ) );
    m_mediaPlayer->stop();
}
