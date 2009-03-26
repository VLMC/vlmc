/*****************************************************************************
 * MediaListWidget.cpp: Multimedia list widget
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Christophe Courtaut <christophe.courtaut@gmail.com>
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


#include "MediaListWidget.h"

MediaListWidget::MediaListWidget( QWidget* parent ) : QListWidget( parent )
{
}

void    MediaListWidget::setType( QString fileType )
{
    this->m_Type = fileType;
}

void    MediaListWidget::mousePressEvent( QMouseEvent* event )
{
    if ( event->button() == Qt::LeftButton)
        this->m_dragStartPos = event->pos();
    QListWidget::mousePressEvent( event );
}

void    MediaListWidget::mouseMoveEvent( QMouseEvent* event )
{
    if ( this->currentItem() == NULL )
        return;
    if ( !( event->buttons() & Qt::LeftButton ) )
         return;
    if ( ( event->pos() - m_dragStartPos ).manhattanLength()
          < QApplication::startDragDistance() )
        return;

    QMimeData* mimeData = new QMimeData;
    mimeData->setText( ( ( ListViewMediaItem* )( currentItem() ) )->fileInfo()->absoluteFilePath() );
    QDrag* drag = new QDrag( this );
    drag->setMimeData( mimeData );
    drag->exec( Qt::CopyAction | Qt::MoveAction, Qt::CopyAction );
}

