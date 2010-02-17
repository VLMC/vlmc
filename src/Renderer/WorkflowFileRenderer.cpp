/*****************************************************************************
 * WorkflowFileRenderer.cpp: Output the workflow to a file
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#include "vlmc.h"
#include "WorkflowFileRenderer.h"
#include "SettingsManager.h"
#include "VLCMedia.h"
#include "LightVideoFrame.h"
#include "VLCMediaPlayer.h"
#include "export/RendererSettings.h"

#include <QTime>

WorkflowFileRenderer::WorkflowFileRenderer() :
        WorkflowRenderer(),
        m_image( NULL )
{
}

WorkflowFileRenderer::~WorkflowFileRenderer()
{
    delete m_image;
}

void        WorkflowFileRenderer::run()
{
//    char        buffer[256];

    m_mainWorkflow->setCurrentFrame( 0, MainWorkflow::Renderer );

    //Setup dialog box for querying render parameters.
    RendererSettings    *settings = new RendererSettings;
    if ( settings->exec() == QDialog::Rejected )
    {
        delete settings;
        return ;
    }
    m_outputFileName = settings->outputFileName();
    quint32     width = settings->width();
    quint32     height = settings->height();
    double      fps = settings->fps();
    quint32     vbitrate = settings->videoBitrate();
    quint32     abitrate = settings->audioBitrate();
    delete settings;

    setupRenderer( width, height, fps );
    setupDialog( width, height );

    //Media as already been created and mainly initialized by the WorkflowRenderer
    QString     transcodeStr = ":sout=#transcode{vcodec=h264,vb=" + QString::number( vbitrate ) +
                               ",acodec=a52,ab=" + QString::number( abitrate ) +
                               ",no-hurry-up}"
                               ":standard{access=file,mux=ps,dst=\""
                          + m_outputFileName + "\"}";
    m_media->addOption( transcodeStr.toStdString().c_str() );

//    sprintf( buffer, ":sout-transcode-fps=%f", m_outputFps );
//    m_media->addOption( buffer );

    m_mediaPlayer->setMedia( m_media );

    connect( m_mainWorkflow, SIGNAL( mainWorkflowEndReached() ), this, SLOT( stop() ) );
    connect( m_mainWorkflow, SIGNAL( frameChanged( qint64, MainWorkflow::FrameChangedReason) ),
             this, SLOT( __frameChanged( qint64,MainWorkflow::FrameChangedReason ) ) );

    m_isRendering = true;
    m_stopping = false;
    m_paused = false;
    m_pts = 0;
    m_audioPts = 0;

    m_mainWorkflow->setFullSpeedRender( true );
    m_mainWorkflow->startRender( width, height );
    m_mediaPlayer->play();
}

void    WorkflowFileRenderer::stop()
{
    WorkflowRenderer::killRenderer();
}

void    WorkflowFileRenderer::cancelButtonClicked()
{
    stop();
    disconnect();
    m_dialog->done( 0 );
}

void
WorkflowFileRenderer::__endReached()
{
     cancelButtonClicked();
}

float   WorkflowFileRenderer::getFps() const
{
    return m_outputFps;
}

int
WorkflowFileRenderer::lock( void *datas, qint64 *dts, qint64 *pts, quint32 *flags,
                            size_t *bufferSize, void **buffer )
{
    int         ret = WorkflowRenderer::lock( datas, dts, pts, flags, bufferSize, buffer );
    EsHandler*  handler = reinterpret_cast<EsHandler*>( datas );
    WorkflowFileRenderer* self = static_cast<WorkflowFileRenderer*>( handler->self );

    if ( self->m_time.isValid() == false ||
        self->m_time.elapsed() >= 1000 )
    {
        self->emit imageUpdated( (uchar*)self->m_renderVideoFrame );
        self->m_time.restart();
    }
    return ret;
}

void        
WorkflowFileRenderer::unlock( void *datas, size_t size, void* buff )
{
    WorkflowRenderer::unlock( datas, size, buff );
}

void        
WorkflowFileRenderer::__frameChanged( qint64 frame, MainWorkflow::FrameChangedReason )
{
    m_dialog->setProgressBarValue( frame * 100 / m_mainWorkflow->getLengthFrame() );
}

void*       
WorkflowFileRenderer::getLockCallback()
{
    return (void*)&WorkflowFileRenderer::lock;
}

void*       
WorkflowFileRenderer::getUnlockCallback()
{
    return (void*)&WorkflowFileRenderer::unlock;
}

quint32
WorkflowFileRenderer::width() const
{
    QVariant width = SettingsManager::getInstance()->value( "project/VideoProjectWidth", 480, SettingsManager::XML );
    return width.toUInt();
}

quint32
WorkflowFileRenderer::height() const
{
    QVariant height = SettingsManager::getInstance()->value( "project/VideoProjectHeight", 300, SettingsManager::XML );
    return height.toUInt();
}

void
WorkflowFileRenderer::setupDialog( quint32 width, quint32 height )
{
    m_dialog = new WorkflowFileRendererDialog( width, height );
    m_dialog->setModal( true );
    m_dialog->setOutputFileName( m_outputFileName );
    connect( m_dialog->m_ui.cancelButton, SIGNAL( clicked() ), this, SLOT( cancelButtonClicked() ) );
    connect( m_dialog, SIGNAL( finished(int) ), this, SLOT( stop() ) );
    connect( this, SIGNAL( imageUpdated( const uchar* ) ),
             m_dialog, SLOT( updatePreview( const uchar* ) ),
             Qt::QueuedConnection );
    m_dialog->show();
}
