/*****************************************************************************
 * PreviewWidget.cpp : Main widget for preview. Will dispatch on Clip or
 *                     Render preview
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

#include <QtDebug>
#include "PreviewWidget.h"
#include "ui_PreviewWidget.h"
#include "ClipRenderer.h"

PreviewWidget::PreviewWidget( GenericRenderer* genericRenderer, QWidget *parent ) :
    QWidget( parent ),
    m_ui( new Ui::PreviewWidget ),
    m_renderer( genericRenderer ),
    m_previewStopped( true )
{
    m_ui->setupUi( this );

    m_ui->rulerWidget->setFocusPolicy( Qt::NoFocus );

    // Prepare and set the black background
    m_ui->renderWidget->setAutoFillBackground( true );
    m_videoPalette = m_ui->renderWidget->palette();
    m_videoPalette.setColor( QPalette::Window, QColor( Qt::black ) );
    m_ui->renderWidget->setPalette( m_videoPalette );

    // Hide markers and createClip buttons if we are not using a ClipRenderer
    if ( !qobject_cast<ClipRenderer*>( genericRenderer ) )
    {
        m_ui->pushButtonMarkerStart->hide();
        m_ui->pushButtonMarkerStop->hide();
        m_ui->pushButtonCreateClip->hide();
    }

    // Give the renderer to the ruler
    m_ui->rulerWidget->setRenderer( m_renderer );

    setAcceptDrops( false );

    connect( m_ui->rulerWidget, SIGNAL( timeChanged(int,int,int,int) ),
             m_ui->lcdNumber,   SLOT( setTime(int,int,int,int) ) );

    m_renderer->setRenderWidget( m_ui->renderWidget );
    m_renderer->setPreviewLabel( m_ui->previewLabel );
    m_ui->previewLabel->hide();

    connect( m_renderer,     SIGNAL( stopped() ),               this,       SLOT( videoStopped() ) );
    connect( m_renderer,     SIGNAL( paused() ),                this,       SLOT( videoPaused() ) );
    connect( m_renderer,     SIGNAL( playing() ),               this,       SLOT( videoPlaying() ) );
    connect( m_renderer,     SIGNAL( frameChanged(qint64, MainWorkflow::FrameChangedReason) ),
             this, SLOT( frameChanged(qint64, MainWorkflow::FrameChangedReason ) ) );
    connect( m_renderer,     SIGNAL( endReached() ),            this,       SLOT( endReached() ) );
    connect( m_ui->rulerWidget, SIGNAL( frameChanged(qint64, MainWorkflow::FrameChangedReason) ),
             m_renderer,       SLOT( previewWidgetCursorChanged(qint64) ) );

    connect( m_ui->pushButtonMarkerStart, SIGNAL( clicked() ), this, SLOT( markerStartClicked() ) );
    connect( m_ui->pushButtonMarkerStop, SIGNAL( clicked() ), this, SLOT( markerStopClicked() ) );
    connect( m_ui->pushButtonCreateClip, SIGNAL( clicked() ), this, SLOT( createNewClipFromMarkers() ) );
}

PreviewWidget::~PreviewWidget()
{
    delete m_renderer;
    delete m_ui;
}

void    PreviewWidget::changeEvent( QEvent *e )
{
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        m_ui->retranslateUi( this );
        break;
    default:
        break;
    }
}

void    PreviewWidget::frameChanged( qint64 currentFrame, MainWorkflow::FrameChangedReason reason )
{
    if ( m_previewStopped == false && reason != MainWorkflow::PreviewCursor )
    {
        m_ui->rulerWidget->setFrame( currentFrame, false );
    }
}

void    PreviewWidget::on_pushButtonStop_clicked()
{
    if ( m_previewStopped == false )
    {
        m_previewStopped = true;
        m_renderer->stop();
    }
}

void    PreviewWidget::on_pushButtonPlay_clicked()
{
    if ( m_previewStopped == true )
        m_previewStopped = false;
    m_renderer->togglePlayPause();
}

void    PreviewWidget::videoPaused()
{
    m_ui->pushButtonPlay->setIcon( QIcon( ":/images/play" ) );
}

void    PreviewWidget::videoStopped()
{
    m_ui->pushButtonPlay->setIcon( QIcon( ":/images/play" ) );
}

void    PreviewWidget::videoPlaying()
{
    m_ui->pushButtonPlay->setIcon( QIcon( ":/images/pause" ) );
}

void    PreviewWidget::endReached()
{
    m_previewStopped = true;

    m_ui->pushButtonPlay->setIcon( QIcon( ":/images/play" ) );

    // Set the black background
    m_ui->renderWidget->setPalette( m_videoPalette );
}

void        PreviewWidget::on_pushButtonNextFrame_clicked()
{
    if ( m_previewStopped == false )
        m_renderer->nextFrame();
}

void        PreviewWidget::on_pushButtonPreviousFrame_clicked()
{
    if ( m_previewStopped == false )
        m_renderer->previousFrame();
}

const GenericRenderer*      PreviewWidget::getGenericRenderer() const
{
    return m_renderer;
}

void                        PreviewWidget::stop()
{
    //Ugly but avoid code dupplication.
    on_pushButtonStop_clicked();
}

void                        PreviewWidget::markerStartClicked()
{
    m_ui->rulerWidget->setMarker( PreviewRuler::START );
}

void                        PreviewWidget::markerStopClicked()
{
    m_ui->rulerWidget->setMarker( PreviewRuler::STOP );
}

void        PreviewWidget::createNewClipFromMarkers()
{
    Media*  selectedMedia = m_renderer->getMedia();
    qint64  beg = m_ui->rulerWidget->getMarker( PreviewRuler::START );
    qint64  end = m_ui->rulerWidget->getMarker( PreviewRuler::STOP );

    if ( beg == -1 && end == -1 )
        return ;

    if ( end < beg )
        return ;

    beg = beg < 0 ? 0 : beg;
    Clip*   part = new Clip( selectedMedia, beg, end );

    //Adding the newly created clip to the media
    selectedMedia->addClip( part );
    emit addClip( part );
    return ;
}
