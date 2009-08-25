#include "WorkflowFileRenderer.h"

WorkflowFileRenderer::WorkflowFileRenderer( const QString& outputFileName ) :
        m_outputFileName( outputFileName )
{
    m_dialog = new WorkflowFileRendererDialog;
    m_dialog->setOutputFileName( outputFileName );
    m_mediaPlayer = new LibVLCpp::MediaPlayer;
}

WorkflowFileRenderer::~WorkflowFileRenderer()
{
}

void        WorkflowFileRenderer::run()
{
    char        buffer[256];

    //Media as already been created an mainly initialized by the WorkflowRenderer
    m_media->addOption( ":no-audio" );
    m_media->addOption( ":fake" );
    sprintf(buffer, ":fake-duration=%lli", m_mainWorkflow->getLength() );
//    m_media->addOption( buffer );

    QString     transcodeStr = "sout=#transcode{vcodec=mp4v,vb=800,acodec=mpga,ab=128" +
                           QString::number( FPS ) + "}"
                               ":standard{access=file,mux=ps,dst=\""
                          + m_outputFileName + "\"}";
    m_media->addOption( transcodeStr.toStdString().c_str() );

    m_mediaPlayer->setMedia( m_media );

    connect( m_mediaPlayer, SIGNAL( stopped() ),    this,   SLOT( stop() ) );
    connect( m_mainWorkflow, SIGNAL( mainWorkflowEndReached() ), this, SLOT( stop() ) );
    connect( m_mainWorkflow, SIGNAL( positionChanged( float ) ), this, SLOT( positionChanged( float ) ) );

    m_dialog->show();

    m_mainWorkflow->startRender();
    m_mediaPlayer->play();
}

void    WorkflowFileRenderer::stop()
{
    qDebug() << "Stopping file renderer";
    WorkflowRenderer::stop();
    m_dialog->done( 0 );
}

void    WorkflowFileRenderer::positionChanged( float newPos )
{
    m_dialog->setProgressBarValue( static_cast<int>( newPos * 100 ) );
}

void    WorkflowFileRenderer::on_cancelButton_clicked()
{
    stop();
}
