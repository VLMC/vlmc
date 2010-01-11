/*****************************************************************************
 * MediaCellView.cpp
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Thomas Boquet <thomas.boquet@gmail.com>
 * Authors: Clement CHAVANCE <chavance.c@gmail.com>
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

#include "MediaCellView.h"
#include "ui_MediaCellView.h"
#include "Library.h"
#include "ClipProperty.h"

#include <QTime>
#include <QDebug>

MediaCellView::MediaCellView( const QUuid& uuid, QWidget *parent ) :
        QWidget( parent ), m_ui( new Ui::MediaCellView ), m_uuid( uuid )
{
    m_ui->setupUi( this );
    setFocusPolicy( Qt::ClickFocus );
    setAutoFillBackground( true );
    connect( m_ui->delLabel, SIGNAL( clicked( QWidget*, QMouseEvent* ) ), this, SLOT( deleteButtonClicked( QWidget*, QMouseEvent* ) ) );
    //TODO : if it's a clip, disable the arrow
    connect( m_ui->arrow,
             SIGNAL( clicked( QWidget*, QMouseEvent* ) ),
             SLOT( arrowButtonClicked( QWidget*, QMouseEvent* ) ) );
}

MediaCellView::~MediaCellView()
{
    delete m_ui;
}

void MediaCellView::changeEvent( QEvent *e )
{
    QWidget::changeEvent( e );
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        m_ui->retranslateUi( this );
        break;
    default:
        break;
    }
}

void            MediaCellView::setTitle( const QString& title )
{
    m_ui->title->setText( title );
}

void            MediaCellView::setThumbnail( const QPixmap& pixmap )
{
    m_ui->thumbnail->setScaledContents( false );
    m_ui->thumbnail->setPixmap( pixmap.scaled( 64, 64, Qt::KeepAspectRatio ) );
}

const QPixmap*  MediaCellView::getThumbnail() const
{
    return m_ui->thumbnail->pixmap();
}

QString  MediaCellView::title() const
{
    return m_ui->title->text();
}

const QUuid&    MediaCellView::uuid() const
{
    return m_uuid;
}

void            MediaCellView::mouseDoubleClickEvent( QMouseEvent* event )
{
    if ( ( event->buttons() | Qt::LeftButton ) == Qt::LeftButton )
    {
        ClipProperty* mp = new ClipProperty( Library::getInstance()->getClip( m_uuid ), this );
        mp->setModal( true );
        mp->show();
    }
}

void            MediaCellView::mousePressEvent( QMouseEvent* event )
{
    QWidget::mousePressEvent( event );

    if ( ( event->buttons() | Qt::LeftButton ) == Qt::LeftButton )
    {
        m_dragStartPos = event->pos();
        emit cellSelected( m_uuid );
    }
}

void    MediaCellView::mouseMoveEvent( QMouseEvent* event )
{
    if ( ( event->buttons() | Qt::LeftButton ) != Qt::LeftButton )
         return;

    if ( ( event->pos() - m_dragStartPos ).manhattanLength()
          < QApplication::startDragDistance() )
        return;

    QMimeData* mimeData = new QMimeData;
    //FIXME the second argument is a media UUID instead of a Clip
    // and this is not logical... but it works.
    mimeData->setData( "vlmc/uuid", m_uuid.toString().toAscii() );
    QDrag* drag = new QDrag( this );
    drag->setMimeData( mimeData );
    //FIXME : change the way the library handles Clips
    Clip* clip = Library::getInstance()->getClip( m_uuid );
    if ( 0 == clip )
        return ;
    //getting the media from the current Clip
    Media*  parent = Library::getInstance()->getClip( m_uuid )->getParent();
    if ( 0 == parent )
        return ;
    drag->setPixmap( Library::getInstance()->getClip( m_uuid )->getParent()->getSnapshot().scaled( 100, 100, Qt::KeepAspectRatio ) );
    drag->exec( Qt::CopyAction | Qt::MoveAction, Qt::CopyAction );
}

const ClickableLabel*   MediaCellView::nextButton() const
{
    return m_ui->arrow;
}

const ClickableLabel*   MediaCellView::deleteButton() const
{
    return m_ui->delLabel;
}

void        MediaCellView::deleteButtonClicked( QWidget*, QMouseEvent* )
{
    emit cellDeleted( uuid() );
}

void        MediaCellView::arrowButtonClicked( QWidget*, QMouseEvent* )
{
    emit arrowClicked( uuid() );
}

void        MediaCellView::setLength( qint64 length, bool mSecs )
{
    QTime   duration;
    if ( mSecs )
        duration = duration.addMSecs( length );
    else
        duration = duration.addSecs( length );
    m_ui->length->setText( duration.toString( "hh:mm:ss" ) );
}

void        MediaCellView::incrementClipCount()
{
    int clips = m_ui->clipCount->text().toInt();

    clips += 1;
    m_ui->clipCount->setText( QString::number( clips ) );
}

void        MediaCellView::decrementClipCount( const int nb )
{
    int clips = m_ui->clipCount->text().toInt();

    clips -= nb;
    if ( clips < 0 )
        clips = 0;
    m_ui->clipCount->setText( QString::number( clips ) );
}
