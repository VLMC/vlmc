/*****************************************************************************
 * WorkflowFileRenderer.h: Output the workflow to a file
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

#ifndef WORKFLOWFILERENDERER_H
#define WORKFLOWFILERENDERER_H

#include "Workflow/MainWorkflow.h"
#include "WorkflowRenderer.h"
#include "WorkflowFileRendererDialog.h"

#include <QTime>

class   WorkflowFileRenderer : public WorkflowRenderer
{
    Q_OBJECT
    Q_DISABLE_COPY( WorkflowFileRenderer )

public:
    WorkflowFileRenderer( const QString& outputFileName );
    virtual ~WorkflowFileRenderer();

    static int          lock( void* datas, qint64 *dts, qint64 *pts, quint32 *flags,
                              size_t *bufferSize, void **buffer );
    static void         unlock( void* datas, size_t size, void* buff );

    void                run();
    virtual float       getFps() const;

private:
    void                setupDialog();
private:
    const QString               m_outputFileName;
    WorkflowFileRendererDialog* m_dialog;
    QImage*                     m_image;
    QTime                       m_time;

protected:
    virtual void*               getLockCallback();
    virtual void*               getUnlockCallback();
    virtual quint32             width() const;
    virtual quint32             height() const;
private slots:
    void                        stop();
    void                        cancelButtonClicked();
    void                        __frameChanged( qint64 frame,
                                                MainWorkflow::FrameChangedReason reason );
    void                        __endReached();

signals:
    void                        imageUpdated( const uchar* image );
};

#endif // WORKFLOWFILERENDERER_H
