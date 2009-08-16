#include "WorkflowFileRenderer.h"

WorkflowFileRenderer::WorkflowFileRenderer( QWidget* parent, const QString& outputFileName ) :
        QDialog( parent ), m_outputFileName( outputFileName )
{
    m_ui.setupUi( this );
    m_ui.nameLabel->setText( outputFileName );
    m_mediaPlayer = new LibVLCpp::MediaPlayer;
    m_mainWorkflow = MainWorkflow::getInstance();
}

WorkflowFileRenderer::~WorkflowFileRenderer()
{
    delete m_mediaPlayer;
}

void*       WorkflowFileRenderer::lock( void* datas )
{
    WorkflowFileRenderer* self = reinterpret_cast<WorkflowFileRenderer*>( datas );
    return self->m_mainWorkflow->getSynchroneOutput();
}

void        WorkflowFileRenderer::unlock( void* )
{
}

void        WorkflowFileRenderer::run()
{
    m_media = new LibVLCpp::Media( "fake://" );
    char        buffer[256];

    sprintf( buffer, ":invmem-width=%i", VIDEOWIDTH );
    m_media->addOption( ":codec=invmem" );
    m_media->addOption( buffer );
    sprintf( buffer, ":invmem-height=%i", VIDEOHEIGHT );
    m_media->addOption( buffer );
    sprintf( buffer, ":invmem-lock=%lld", (qint64)WorkflowFileRenderer::lock );
    m_media->addOption( buffer );
    sprintf( buffer, ":invmem-unlock=%lld", (qint64)WorkflowFileRenderer::unlock );
    m_media->addOption( buffer );
    sprintf( buffer, ":invmem-data=%lld", (qint64)this );
    m_media->addOption( buffer );
    m_media->addOption( ":no-audio" );
    m_media->addOption( ":fake" );
    sprintf(buffer, ":fake-duration=%lli", m_mainWorkflow->getLength() );
//    m_media->addOption( buffer );

    QString     transcodeStr = "sout=#transcode{vcodec=mp4v,vb=800,acodec=mpga,ab=128}"
                               ":standard{access=file,mux=ps,dst=\""
                          + m_outputFileName + "\"}";
    m_media->addOption( transcodeStr.toStdString().c_str() );
    
    m_mediaPlayer->setMedia( m_media );

    connect( m_mediaPlayer, SIGNAL( stopped() ),    this,   SLOT( stop() ) );
    connect( m_mainWorkflow, SIGNAL( mainWorkflowEndReached() ), this, SLOT( stop() ) );
    connect( m_mainWorkflow, SIGNAL( positionChanged( float ) ), this, SLOT( positionChanged( float ) ) );

    show();

    m_mainWorkflow->startRender();
    m_mediaPlayer->play();
}

void    WorkflowFileRenderer::stop()
{
    disconnect( m_mediaPlayer, SIGNAL( stopped() ),    this,   SLOT( stop() ) );
    disconnect( m_mainWorkflow, SIGNAL( mainWorkflowEndReached() ), this, SLOT( stop() ) );

    m_mainWorkflow->stop();
    m_mediaPlayer->stop();
    done( 0 );
}

void    WorkflowFileRenderer::positionChanged( float newPos )
{
    m_ui.progressBar->setValue( static_cast<int>( newPos * 100 ) );
}

void    WorkflowFileRenderer::on_cancelButton_clicked()
{
    stop();
}
