/*****************************************************************************
 * PreviewWidget: Main preview widget
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


#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QWidget>
#include "Workflow/MainWorkflow.h"
#include "ClipPreviewWidget.h"
#include "RenderPreviewWidget.h"

namespace Ui {
    class PreviewWidget;
}

class   PreviewWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY( PreviewWidget )

public:
    explicit PreviewWidget( MainWorkflow* mainWorkflow, QWidget* parent = NULL );
    virtual ~PreviewWidget();

private:
    Ui::PreviewWidget*      m_ui;
    GenericPreviewWidget*   m_clipPreview;
    GenericPreviewWidget*   m_renderPreview;
    GenericPreviewWidget*   m_currentPreviewRenderer;
    bool                    m_endReached;
    bool                    m_previewStopped;
    int                     m_currentMode;

    static const int        renderPreviewMode = 0;
    static const int        clipPreviewMode = 1;

private:


protected:
    virtual void    changeEvent( QEvent *e );
    virtual void    dragEnterEvent( QDragEnterEvent* event );
    virtual void    dropEvent( QDropEvent* event );
private slots:
    void            on_pushButtonPlay_clicked();
    void            positionChanged( float );
    void            seekSliderPressed();
    void            seekSliderMoved( int value );
    void            seekSliderReleased();
    void            videoPaused();
    void            videoPlaying();
    void            endReached();

    void            changedTab( int );
};

#endif // PREVIEWWIDGET_H
