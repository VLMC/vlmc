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

#include <QUrl>
#include <QtDebug>
#include <QMetaMethod>
#include "PreviewWidget.h"
#include "ui_PreviewWidget.h"
#include "MediaListWidget.h"
#include "Library.h"

PreviewWidget::PreviewWidget( GenericRenderer* genericRenderer, QWidget *parent ) :
    QWidget( parent ),
    m_ui( new Ui::PreviewWidget ),
    m_renderer( genericRenderer ),
    m_previewStopped( true )
{
    m_ui->setupUi( this );

    m_ui->rulerWidget->setMinimum( 0 );
    m_ui->rulerWidget->setMaximum( 1000 );
    m_ui->rulerWidget->setSingleStep( 2 );
    m_ui->rulerWidget->setFocusPolicy( Qt::NoFocus );

    // Prepare and set the black background
    m_ui->renderWidget->setAutoFillBackground( true );
    m_videoPalette = m_ui->renderWidget->palette();
    m_videoPalette.setColor( QPalette::Window, QColor( Qt::black ) );
    m_ui->renderWidget->setPalette( m_videoPalette );

    // Give the renderer to the ruler
    m_ui->rulerWidget->setRenderer( m_renderer );

    setAcceptDrops( false );

    connect( m_ui->rulerWidget, SIGNAL( sliderPressed() ),       this,   SLOT( seekSliderPressed() ) );
    connect( m_ui->rulerWidget, SIGNAL( sliderPosChanged(int) ), this,   SLOT( seekSliderMoved(int) ) );
    connect( m_ui->rulerWidget, SIGNAL( sliderReleased() ),      this,   SLOT( seekSliderReleased() ) );
    connect( m_ui->rulerWidget, SIGNAL( timeChanged(int,int,int,int) ),
             m_ui->lcdNumber,   SLOT( setTime(int,int,int,int) ) );

    m_renderer->setRenderWidget( m_ui->renderWidget );
    m_renderer->setPreviewLabel( m_ui->previewLabel );
    m_ui->previewLabel->hide();

    connect( m_renderer,     SIGNAL( stopped() ),                this,       SLOT( videoStopped() ) );
    connect( m_renderer,     SIGNAL( paused() ),                 this,       SLOT( videoPaused() ) );
    connect( m_renderer,     SIGNAL( playing() ),                this,       SLOT( videoPlaying() ) );
    connect( m_renderer,     SIGNAL( positionChanged(float) ),   this,       SLOT( positionChanged(float) ) );
    connect( m_renderer,     SIGNAL( endReached() ),             this,       SLOT( endReached() ) );
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

void    PreviewWidget::positionChanged( float newPos )
{
    if ( m_previewStopped == false )
        m_ui->rulerWidget->setValue( (int)( newPos * 1000.0 ) );
}


void    PreviewWidget::seekSliderPressed()
{
    disconnect( m_renderer, SIGNAL( positionChanged( float ) ),
                this, SLOT( positionChanged( float ) ) );
}

void    PreviewWidget::seekSliderMoved( int )
{
    if ( m_ui->rulerWidget->value() == m_ui->rulerWidget->maximum() )
    {
        m_endReached = true;
        return;
    }
    m_endReached = false;
    //Putting back the slider value into vlc position
    m_renderer->setPosition( (float)m_ui->rulerWidget->value() / 1000.0f );
}

void    PreviewWidget::seekSliderReleased()
{
    if ( m_endReached == true && m_previewStopped == false )
    {
        //When cursor reaches the maximum right, end reached becomes true.
        //When we will release our slider, if endReached is true, we actually set the position.
        //Otherwise, we do nothing.
        //This prevents the video to stop if we put the slider to the maximum right by mistake
        m_renderer->setPosition( (float)m_ui->rulerWidget->maximum() );
        m_previewStopped = false;
    }
    connect( m_renderer, SIGNAL( positionChanged( float ) ),
             this, SLOT( positionChanged( float ) ) );
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
    m_ui->rulerWidget->setValue( 0 );
}

void    PreviewWidget::videoPlaying()
{
    m_ui->pushButtonPlay->setIcon( QIcon( ":/images/pause" ) );
}

void    PreviewWidget::endReached()
{
    m_previewStopped = true;

    m_ui->pushButtonPlay->setIcon( QIcon( ":/images/play" ) );
    m_ui->rulerWidget->setValue( 0 );

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
