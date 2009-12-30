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
#include "PreviewRuler.h"

class GenericRenderer;

namespace Ui {
    class PreviewWidget;
}

class   PreviewWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY( PreviewWidget )

public:
    explicit PreviewWidget( GenericRenderer* renderer, QWidget* parent = NULL );
    virtual ~PreviewWidget();
    const GenericRenderer*  getGenericRenderer() const;

private:
    Ui::PreviewWidget*      m_ui;
    GenericRenderer*        m_renderer;
    bool                    m_endReached;
    bool                    m_previewStopped;
    QPalette                m_videoPalette;

protected:
    virtual void    changeEvent( QEvent *e );

public slots:
    void            stop();

private slots:
    void            on_pushButtonPlay_clicked();
    void            on_pushButtonStop_clicked();
    void            on_pushButtonNextFrame_clicked();
    void            on_pushButtonPreviousFrame_clicked();
    void            frameChanged( qint64, MainWorkflow::FrameChangedReason reason );
    void            videoPaused();
    void            videoPlaying();
    void            videoStopped();
    void            endReached();
    void            markerStartClicked();
    void            markerStopClicked();
    void            createNewClipFromMarkers();

signals:
    void            addClip( Clip* clip );
};

#endif // PREVIEWWIDGET_H
