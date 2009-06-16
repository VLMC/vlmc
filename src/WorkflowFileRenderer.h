/*****************************************************************************
 * WorkflowFileRenderer.h: Output the workflow to a file
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

#ifndef WORKFLOWFILERENDERER_H
#define WORKFLOWFILERENDERER_H

#include <QDialog>

#include "VLCMediaPlayer.h"
#include "Workflow/MainWorkflow.h"
#include "ui_WorkflowFileRenderer.h"

class   WorkflowFileRenderer : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY( WorkflowFileRenderer )

public:
    WorkflowFileRenderer( QWidget* parent, const QString& outputFileName );
    virtual ~WorkflowFileRenderer();

    static void*        lock( void* datas );
    static void         unlock( void* datas );

    void                run();
private:
    LibVLCpp::MediaPlayer*      m_mediaPlayer;
    LibVLCpp::Media*            m_media;
    MainWorkflow*               m_mainWorkflow;
    const QString               m_outputFileName;
    Ui::WorkflowFileRenderer    m_ui;

private slots:
    void                        stop();
    void                        positionChanged( float newPos );
    void                        on_cancelButton_clicked();
};

#endif // WORKFLOWFILERENDERER_H
