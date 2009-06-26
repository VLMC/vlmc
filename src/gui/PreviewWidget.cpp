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

#include "ui_PreviewWidget.h"
#include "gui/PreviewWidget.h"
#include "MediaListWidget.h"
#include "Library.h"

PreviewWidget::PreviewWidget( MainWorkflow* mainWorkflow, QWidget *parent ) :
    QWidget( parent ),
    m_ui( new Ui::PreviewWidget ),
    m_currentPreviewRenderer( NULL ),
    m_previewStopped( true ),
    m_sliderPosBackup( 0 )
{
    m_ui->setupUi( this );

    m_ui->seekSlider->setMinimum( 0 );
    m_ui->seekSlider->setMaximum( 1000 );
    m_ui->seekSlider->setSingleStep( 2 );
    m_ui->seekSlider->setFocusPolicy( Qt::NoFocus );

    setAcceptDrops( true );

    connect( m_ui->seekSlider, SIGNAL( sliderPressed() ),       this,   SLOT( seekSliderPressed() ) );
    connect( m_ui->seekSlider, SIGNAL( sliderPosChanged(int) ), this,   SLOT( seekSliderMoved(int) ) );
    connect( m_ui->seekSlider, SIGNAL( sliderReleased() ),      this,   SLOT( seekSliderReleased() ) );

    initClipPreview();
    initRenderPreview( mainWorkflow );

    m_currentMode = m_ui->tabWidget->currentIndex();
    m_currentPreviewRenderer = m_renderPreview;
    connect( m_ui->tabWidget, SIGNAL( currentChanged(int) ), this, SLOT( changedTab(int) ) );
}

PreviewWidget::~PreviewWidget()
{
    delete m_ui;
    delete m_clipPreview;
    delete m_renderPreview;
}

void    PreviewWidget::initClipPreview()
{
    m_clipPreview = new ClipPreviewWidget( m_ui->clipPreviewRenderWidget );
    connectPreview( m_clipPreview );
}

void    PreviewWidget::initRenderPreview( MainWorkflow* mainWorkflow )
{
    m_renderPreview = new RenderPreviewWidget( mainWorkflow, m_ui->renderPreviewRenderWidget );
    connectPreview( m_renderPreview );
}

void    PreviewWidget::connectPreview( GenericPreviewWidget* target )
{
    //WARNING:  the slots must NOT be virtual, since this is called from the constructor
    //          which would be unsafe... if not fatal...
    connect( target,     SIGNAL( stopped() ),                this,       SLOT( videoStopped() ) );
    connect( target,     SIGNAL( paused() ),                 this,       SLOT( videoPaused() ) );
    connect( target,     SIGNAL( playing() ),                this,       SLOT( videoPlaying() ) );
    connect( target,     SIGNAL( positionChanged(float) ),   this,       SLOT( positionChanged(float) ) );
    connect( target,     SIGNAL( endReached() ),             this,       SLOT( endReached() ) );
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

void    PreviewWidget::dragEnterEvent( QDragEnterEvent* event )
{
    if ( event->mimeData()->hasFormat( "vlmc/uuid" ) || 
         event->mimeData()->urls().count() == 1 )
    {
        event->acceptProposedAction();
    }
}

void    PreviewWidget::dropEvent( QDropEvent* event )
{
    //If the dropped event is a clip to preview :
    if ( event->mimeData()->hasFormat( "vlmc/uuid" ) ||
         event->mimeData()->urls().count() == 1 )
    {
        Media* media;
        if ( event->mimeData()->urls().count() == 1 )
        {
            Library* lib = Library::getInstance();
            lib->newMediaLoadingAsked( event->mimeData()->urls()[0].path() );
            media = lib->getMedia( event->mimeData()->urls()[0].path() );
        }
        else
            media = Library::getInstance()->getMedia( QUuid( QString( event->mimeData()->data( "vlmc/uuid" ) ) ) );

        if ( media == NULL )
        {
            qDebug() << "Unknown media" << event->mimeData()->data( "vlmc/uuid" );
            return ;
        }
        if ( m_currentMode != PreviewWidget::clipPreviewMode )
            m_ui->tabWidget->setCurrentIndex( PreviewWidget::clipPreviewMode );

        m_clipPreview->startPreview( media );
        event->acceptProposedAction();
        m_previewStopped = false;
    }
    //else: I don't see how we could drag and drop a workflow :o (at the moment)
}

void    PreviewWidget::positionChanged( float newPos )
{
    if ( m_previewStopped == false )
        m_ui->seekSlider->setValue( (int)( newPos * 1000.0 ) );
}


void    PreviewWidget::seekSliderPressed()
{
    disconnect( m_currentPreviewRenderer, SIGNAL( positionChanged( float ) ),
                this, SLOT( positionChanged( float ) ) );
}

void    PreviewWidget::seekSliderMoved( int )
{
    if ( m_ui->seekSlider->value() == m_ui->seekSlider->maximum() )
    {
        m_endReached = true;
        return;
    }
    m_endReached = false;
    //Putting back the slider value into vlc position
    m_currentPreviewRenderer->setPosition( (float)m_ui->seekSlider->value() / 1000.0f );
}

void    PreviewWidget::seekSliderReleased()
{
    if ( m_endReached == true && m_previewStopped == false )
    {
        //When cursor reaches the maximum right, end reached becomes true.
        //When we will release our slider, if endReached is true, we actually set the position.
        //Otherwise, we do nothing.
        //This prevents the video to stop if we put the slider to the maximum right by mistake
        m_currentPreviewRenderer->setPosition( (float)m_ui->seekSlider->maximum() );
        m_previewStopped = false;
    }
    connect( m_currentPreviewRenderer, SIGNAL( positionChanged( float ) ),
             this, SLOT( positionChanged( float ) ) );
}

void    PreviewWidget::on_pushButtonStop_clicked()
{
    if ( m_previewStopped == false )
    {
        m_previewStopped = true;
        m_currentPreviewRenderer->stop();
    }
}

void    PreviewWidget::on_pushButtonPlay_clicked()
{
    if ( m_previewStopped == true )
        m_previewStopped = false;
//    int     methodIndex = m_currentPreviewRenderer->metaObject()->indexOfMethod("togglePlayPause(bool)");
//    qDebug() << methodIndex;
//    QMetaMethod method = m_currentPreviewRenderer->metaObject()->method( methodIndex );
//    qDebug() << method.invoke( m_currentPreviewRenderer, Qt::QueuedConnection, Q_ARG(bool, false) );
    m_currentPreviewRenderer->togglePlayPause();
}

void    PreviewWidget::videoPaused()
{
    m_ui->pushButtonPlay->setIcon( QIcon( ":/images/play" ) );
}

void    PreviewWidget::videoStopped()
{
    m_ui->pushButtonPlay->setIcon( QIcon( ":/images/play" ) );
    m_ui->seekSlider->setValue( 0 );
}

void    PreviewWidget::videoPlaying()
{
    m_ui->pushButtonPlay->setIcon( QIcon( ":/images/pause" ) );
}

void    PreviewWidget::endReached()
{
    m_previewStopped = true;

    m_ui->pushButtonPlay->setIcon( QIcon( ":/images/play" ) );
    m_ui->seekSlider->setValue( 0 );
}

void    PreviewWidget::changedTab( int tabId )
{
    m_currentPreviewRenderer->togglePlayPause( true );

    if ( tabId == PreviewWidget::clipPreviewMode )
    {
        m_currentPreviewRenderer = m_clipPreview;
    }
    else if ( tabId == PreviewWidget::renderPreviewMode )
    {
        m_currentPreviewRenderer = m_renderPreview;
    }
    else
        qDebug() << "Unknown and uncoherent tabId for PreviewWidget : " << tabId;
    m_currentMode = !m_currentMode;

    int   tmp = m_ui->seekSlider->value();
    m_ui->seekSlider->setValue( m_sliderPosBackup );
    m_sliderPosBackup = tmp;
}

void        PreviewWidget::on_pushButtonNextFrame_clicked()
{
    if ( m_previewStopped == false )
        m_currentPreviewRenderer->nextFrame();
}

void        PreviewWidget::on_pushButtonPreviousFrame_clicked()
{
    if ( m_previewStopped == false )
        m_currentPreviewRenderer->previousFrame();
}
