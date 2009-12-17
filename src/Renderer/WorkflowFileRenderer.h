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

#include "VLCMediaPlayer.h"
#include "Workflow/MainWorkflow.h"
#include "WorkflowRenderer.h"
#include "WorkflowFileRendererDialog.h"

#include <QTimer>

class   WorkflowFileRenderer : public WorkflowRenderer
{
    Q_OBJECT
    Q_DISABLE_COPY( WorkflowFileRenderer )

public:
    WorkflowFileRenderer( const QString& outputFileName );
    virtual ~WorkflowFileRenderer();

    static void*        lock( void* datas );
    static void         unlock( void* datas );

    void                run();
    virtual float       getFps() const;
private:
    const QString               m_outputFileName;
    WorkflowFileRendererDialog* m_dialog;
    QImage*                     m_image;
    QTimer                      m_timer;

protected:
    virtual void*       getLockCallback();
    virtual void*       getUnlockCallback();

private slots:
    void                        stop();
    void                        cancelButtonClicked();
    void                        __frameChanged( qint64 frame, MainWorkflow::FrameChangedReason reason );

signals:
    void                        imageUpdated( const uchar* image );
};

#endif // WORKFLOWFILERENDERER_H
