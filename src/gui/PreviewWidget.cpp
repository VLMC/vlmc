/*****************************************************************************
 * PreviewWidget: Preview widget
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

#include "PreviewWidget.h"
#include "ui_PreviewWidget.h"
#include "MediaListWidget.h"
#include "LibraryWidget.h"

PreviewWidget::PreviewWidget( QWidget *parent ) :
    QDialog( parent ),
    m_ui( new Ui::PreviewWidget )
{
    m_ui->setupUi( this );
    m_ui->groupBoxButton->hide();
    m_ui->seekSlider->setMinimum( 0 );
    m_ui->seekSlider->setMaximum( 1000 );
    m_ui->seekSlider->setSingleStep( 2 );
    m_ui->seekSlider->setFocusPolicy( Qt::NoFocus );

    setAcceptDrops(true);

    m_currentInstance = LibVLCpp::Instance::getInstance();

    connect( m_ui->seekSlider, SIGNAL( sliderPosChanged(int) ),
             this, SLOT( seekSliderMoved(int) ) );

    m_mediaPlayer = new LibVLCpp::MediaPlayer();
}

PreviewWidget::~PreviewWidget()
{
    delete m_ui;
}

void PreviewWidget::changeEvent( QEvent *e )
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
    event->accept();
}

void    PreviewWidget::dropEvent( QDropEvent* event )
{
    Clip*   clip = LibraryWidget::getInstance()->getClip( event->mimeData()->text() );

    m_mediaPlayer->setMedia( clip->getVLCMedia() );
    clip->setupMedia();
    m_mediaPlayer->setDrawable( m_ui->clipRenderWidget->winId() );

    //FIXME Connecting endReached to pause to change icon of playpause button
    // this might not work as it works now later!
    connect( m_mediaPlayer,
             SIGNAL( endReached() ),
             this,
             SLOT ( videoPaused() ) );
    connect( m_mediaPlayer,
             SIGNAL( stopped() ),
             this,
             SLOT ( videoPaused() ) );
    connect( m_mediaPlayer,
             SIGNAL( playing() ),
             this,
             SLOT ( videoPlaying() ) );
    connect( m_mediaPlayer,
             SIGNAL( positionChanged() ),
             this,
             SLOT( positionChanged() ) );

    m_mediaPlayer->play();
}

void    PreviewWidget::positionChanged()
{
   //TODO
}

void    PreviewWidget::seekSliderMoved( int )
{
    //TODO
}

void PreviewWidget::on_pushButtonPlay_clicked()
{
    //TODO
}

void PreviewWidget::videoPaused()
{
    m_ui->pushButtonPlay->setIcon( QIcon( ":/images/play" ) );
}

void PreviewWidget::videoPlaying()
{
    m_ui->pushButtonPlay->setIcon( QIcon( ":/images/pause" ) );
}
