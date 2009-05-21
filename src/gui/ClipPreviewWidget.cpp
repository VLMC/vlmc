/*****************************************************************************
 * ClipPreviewWidget: Preview widget
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Geoffroy Lacarrière <geoffroylaca@gmail.com>
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
#include <QUrl>

#include "ClipPreviewWidget.h"
#include "ui_ClipPreviewWidget.h"
#include "MediaListWidget.h"
#include "Library.h"


ClipPreviewWidget::ClipPreviewWidget( QWidget *parent ) :
    QDialog( parent ),
    m_ui( new Ui::ClipPreviewWidget ), m_clipLoaded( false ), m_videoStopped( true )
{
    m_ui->setupUi( this );
    m_ui->groupBoxButton->hide();
    m_ui->seekSlider->setMinimum( 0 );
    m_ui->seekSlider->setMaximum( 1000 );
    m_ui->seekSlider->setSingleStep( 2 );
    m_ui->seekSlider->setFocusPolicy( Qt::NoFocus );

    setAcceptDrops( true );

    connect( m_ui->seekSlider, SIGNAL( sliderPressed() ),       this,   SLOT( seekSliderPressed() ) );
    connect( m_ui->seekSlider, SIGNAL( sliderPosChanged(int) ), this,   SLOT( seekSliderMoved(int) ) );
    connect( m_ui->seekSlider, SIGNAL( sliderReleased() ),      this,   SLOT( seekSliderReleased() ) );

    m_mediaPlayer = new LibVLCpp::MediaPlayer();
    m_mediaPlayer->setDrawable( m_ui->clipRenderWidget->winId() );

//    m_mediaList = new LibVLCpp::MediaList();
//    m_mediaList->setMediaPlayer( m_mediaPlayer );
}

ClipPreviewWidget::~ClipPreviewWidget()
{
    delete m_mediaPlayer;
    delete m_ui;
}

void ClipPreviewWidget::changeEvent( QEvent *e )
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

void    ClipPreviewWidget::dragEnterEvent( QDragEnterEvent* event )
{
    if ( event->mimeData()->hasFormat( "vlmc/uuid" ) )
        event->acceptProposedAction();
    else if ( event->mimeData()->urls().count() == 1 )
        event->acceptProposedAction();
}

void    ClipPreviewWidget::dropEvent( QDropEvent* event )
{
    Media* media;
    if ( event->mimeData()->urls().count() == 1 )
    {
        Library* lib = Library::getInstance();
        lib->newMediaLoadingAsked( event->mimeData()->urls()[0].path() );
        media = lib->getMedia( event->mimeData()->urls()[0].path() );
    }
    else
        media = Library::getInstance()->getMedia( QUuid( ( const QString& )event->mimeData()->data( "vlmc/uuid" ) ) );

    media->flushParameters();
    m_mediaPlayer->setMedia( media->getVLCMedia() );

    connect( m_mediaPlayer,     SIGNAL( stopped() ),            this,       SLOT( videoPaused() ) );
    connect( m_mediaPlayer,     SIGNAL( paused() ),             this,       SLOT( videoPaused() ) );
    connect( m_mediaPlayer,     SIGNAL( playing() ),            this,       SLOT( videoPlaying() ) );
    connect( m_mediaPlayer,     SIGNAL( positionChanged() ),    this,       SLOT( positionChanged() ) );
    connect( m_mediaPlayer,     SIGNAL( endReached() ),         this,       SLOT( endReached() ) );

    m_mediaPlayer->play();
    m_clipLoaded = true;
    m_videoStopped = false;
    event->acceptProposedAction();
}

void    ClipPreviewWidget::positionChanged()
{
    if ( m_clipLoaded == false)
        return ;
    m_ui->seekSlider->setValue( (int)( m_mediaPlayer->getPosition() * 1000.0 ) );
}


void    ClipPreviewWidget::seekSliderPressed()
{
    disconnect( m_mediaPlayer,     SIGNAL( positionChanged() ),    this,       SLOT( positionChanged() ) );
}

void    ClipPreviewWidget::seekSliderMoved( int )
{
    if ( m_clipLoaded == false || m_videoStopped == true )
        return ;
    if ( m_ui->seekSlider->value() == m_ui->seekSlider->maximum() )
    {
        m_endReached = true;
        return;
    }
    m_endReached = false;
    m_mediaPlayer->setPosition( (float)m_ui->seekSlider->value() / 1000.0 );
}

void    ClipPreviewWidget::seekSliderReleased()
{
    if ( m_endReached == true && m_videoStopped == false )
    {
        //When cursor reaches the maximum right, end reached becomes true.
        //When we will release our slider, if endReached is true, we actually set the position.
        //Otherwise, we do nothing.
        //This prevents the video to stop if we put the slider to the maximum right by mistake
        m_mediaPlayer->setPosition( (float)m_ui->seekSlider->maximum() / 1000.0 );
        m_endReached = false;
    }
    connect( m_mediaPlayer,     SIGNAL( positionChanged() ),    this,       SLOT( positionChanged() ) );
}

void ClipPreviewWidget::on_pushButtonPlay_clicked()
{
    if ( m_clipLoaded == false)
        return ;
    if ( m_videoStopped == true )
        m_videoStopped = false;

    if ( m_mediaPlayer->isPlaying() )
        m_mediaPlayer->pause();
    else
        m_mediaPlayer->play();
}

void ClipPreviewWidget::videoPaused()
{
    m_ui->pushButtonPlay->setIcon( QIcon( ":/images/play" ) );
}

void ClipPreviewWidget::videoPlaying()
{
    m_ui->pushButtonPlay->setIcon( QIcon( ":/images/pause" ) );
}

void    ClipPreviewWidget::endReached()
{
    //Media player part :
    m_mediaPlayer->stop();
    m_videoStopped = true;

    //GUI part :
    m_ui->pushButtonPlay->setIcon( QIcon( ":/images/play" ) );
    m_ui->seekSlider->setValue( 0 );
}

