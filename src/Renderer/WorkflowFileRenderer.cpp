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

#include "WorkflowFileRenderer.h"
#include "SettingsManager.h"

WorkflowFileRenderer::WorkflowFileRenderer( const QString& outputFileName ) :
        m_outputFileName( outputFileName )
{
    m_dialog = new WorkflowFileRendererDialog;
    m_dialog->setModal( true );
    m_dialog->setOutputFileName( outputFileName );
    m_mediaPlayer = new LibVLCpp::MediaPlayer;
    connect( m_dialog->m_ui.cancelButton, SIGNAL( clicked() ), this, SLOT( cancelButtonClicked() ) );
}

WorkflowFileRenderer::~WorkflowFileRenderer()
{
}

void        WorkflowFileRenderer::run()
{
    char        buffer[256];

    m_outputFps = SettingsManager::getInstance()->getValue( "VLMC", "VLMCOutPutFPS" ).toDouble();

    //Media as already been created an mainly initialized by the WorkflowRenderer
    m_media->addOption( ":no-audio" );
    m_media->addOption( "no-sout-audio" );
    m_media->addOption( ":fake" );
    //sprintf(buffer, ":fake-fps=%i", FPS );
    //m_media->addOption( buffer );
    QString     transcodeStr = ":sout=#transcode{vcodec=mp4v,vb=800,acodec=mpga,ab=128,no-hurry-up}"
                               ":standard{access=file,mux=ps,dst=\""
                          + m_outputFileName + "\"}";
    m_media->addOption( transcodeStr.toStdString().c_str() );

    sprintf( buffer, ":sout-transcode-fps=%f", m_outputFps );
    m_media->addOption( buffer );

    m_mediaPlayer->setMedia( m_media );

    connect( m_mainWorkflow, SIGNAL( mainWorkflowEndReached() ), this, SLOT( stop() ) );
    connect( m_mainWorkflow, SIGNAL( positionChanged( float ) ), this, SLOT( positionChanged( float ) ) );

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
    m_dialog->done( 0 );
}

void    WorkflowFileRenderer::cancelButtonClicked()
{
    stop();
}

float   WorkflowFileRenderer::getFps() const
{
    return m_outputFps;
}
