/*****************************************************************************
 * WorkflowFileRenderer.cpp: Output the workflow to a file
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
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

WorkflowFileRenderer::WorkflowFileRenderer( const QString& outputFileName ) :
        WorkflowRenderer(),
        m_outputFileName( outputFileName )
{
    m_image = NULL;
    m_timer.setSingleShot( true );
    m_dialog = new WorkflowFileRendererDialog();
    m_dialog->setModal( true );
    m_dialog->setOutputFileName( outputFileName );
    connect( m_dialog->m_ui.cancelButton, SIGNAL( clicked() ), this, SLOT( cancelButtonClicked() ) );
    connect( m_dialog, SIGNAL( finished(int) ), this, SLOT( stop() ) );
    connect( this, SIGNAL( imageUpdated( const uchar* ) ), m_dialog, SLOT( updatePreview( const uchar* ) ) );
}

WorkflowFileRenderer::~WorkflowFileRenderer()
{
    delete m_image;
}

void        WorkflowFileRenderer::run()
{
//    char        buffer[256];

    m_mainWorkflow->setCurrentFrame( 0, MainWorkflow::Renderer );
    m_outputFps = SettingsManager::getInstance()->getValue( "VLMC", "VLMCOutPutFPS" )->get().toDouble();

    //Media as already been created an mainly initialized by the WorkflowRenderer
    m_media->addOption( ":no-sout-audio" );
    QString     transcodeStr = ":sout=#transcode{vcodec=mp4v,vb=800,acodec=mpga,ab=128,no-hurry-up}"
                               ":standard{access=file,mux=ps,dst=\""
                          + m_outputFileName + "\"}";
    m_media->addOption( transcodeStr.toStdString().c_str() );

//    sprintf( buffer, ":sout-transcode-fps=%f", m_outputFps );
//    m_media->addOption( buffer );

    m_mediaPlayer->setMedia( m_media );

    connect( m_mainWorkflow, SIGNAL( mainWorkflowEndReached() ), this, SLOT( stop() ) );

    m_dialog->show();

    m_isRendering = true;
    m_stopping = false;

    m_mainWorkflow->setFullSpeedRender( true );
    m_mainWorkflow->startRender();
    m_mediaPlayer->play();
}

void    WorkflowFileRenderer::stop()
{
    WorkflowRenderer::stop();
}

void    WorkflowFileRenderer::cancelButtonClicked()
{
    stop();
    m_dialog->done( 0 );
}

float   WorkflowFileRenderer::getFps() const
{
    return m_outputFps;
}

void*       WorkflowFileRenderer::lock( void *datas )
{
    WorkflowFileRenderer* self = reinterpret_cast<WorkflowFileRenderer*>( datas );
    void*   ret = WorkflowRenderer::lock( datas );

    if ( self->m_timer.isActive() == false )
    {
        self->emit imageUpdated( (uchar*)ret );
        self->m_timer.start( 1000 );
    }
    return ret;
}

void        WorkflowFileRenderer::unlock( void *datas )
{
    WorkflowRenderer::unlock( datas );
}

void        WorkflowFileRenderer::__frameChanged( qint64 frame, MainWorkflow::FrameChangedReason )
{
    m_dialog->setProgressBarValue( frame * 100 / m_mainWorkflow->getLengthFrame() );
}

void*       WorkflowFileRenderer::getLockCallback()
{
    return (void*)&WorkflowFileRenderer::lock;
}

void*       WorkflowFileRenderer::getUnlockCallback()
{
    return (void*)&WorkflowFileRenderer::unlock;
}
