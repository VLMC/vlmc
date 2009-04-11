#include "MetaDataManager.h"

MetaDataManager::MetaDataManager() : m_renderWidget( NULL )
{
    m_mediaPlayer = new LibVLCpp::MediaPlayer();
    connect( LibraryWidget::getInstance(), SIGNAL( listViewMediaAdded( Clip* ) ), this, SLOT( listViewMediaAdded( Clip* ) ) );
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

void    MetaDataManager::listViewMediaAdded( Clip* item )
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

            m_currentClip->flushParameters();
            m_mediaPlayer->setMedia( m_currentClip->getVLCMedia() );
            m_mediaPlayer->play();

            connect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( renderSnapshot() ) );
        }
        usleep( 100 );
    }
    return;
}

void    MetaDataManager::renderSnapshot()
{
    m_mediaPlayer->setTime( m_mediaPlayer->getLength() / 3 );

    //FIXME: add a signal / slot for time changed.
    sleep(1);

    QTemporaryFile tmp;
    tmp.setAutoRemove( false );
    tmp.open();

    strncpy(m_tmpSnapshotFilename, tmp.fileName().toStdString().c_str(), tmp.fileName().length());

    connect( m_mediaPlayer, SIGNAL( snapshotTaken() ), this, SLOT( setSnapshotInIcon() ) );

    //The slot should be triggered in this methode
    m_mediaPlayer->takeSnapshot( m_tmpSnapshotFilename, 32, 32 );
    //Snapshot slot should has been called (but maybe not in next version...)
}

void    MetaDataManager::setSnapshotInIcon()
{
    m_currentClip->setSnapshot( new QPixmap( m_tmpSnapshotFilename ) );
    m_nextMedia = true;
    disconnect( this, SLOT( setSnapshotInIcon() ) );
    m_mediaPlayer->stop();
}
