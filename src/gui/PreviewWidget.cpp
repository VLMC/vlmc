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

#include "PreviewWidget.h"
#include "ui_PreviewWidget.h"
#include "MediaListWidget.h"

PreviewWidget::PreviewWidget( QWidget *parent ) :
    QDialog( parent ),
    m_ui( new Ui::PreviewWidget ), m_currentMedia( NULL )
{
    m_ui->setupUi( this );

    char const *vlc_argv[] =
        {
            "-verbose", "3",
            "--no-skip-frames",
            //"--plugin-path", VLC_TREE "/modules",
            //"--ignore-config", //Don't use VLC's config files
        };
    int vlc_argc = sizeof( vlc_argv ) / sizeof( *vlc_argv );
    setAcceptDrops(true);
    m_currentInstance = new LibVLCpp::Instance( vlc_argc, vlc_argv );
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
    QListWidget*    listWidget = reinterpret_cast<QListWidget*>( event->source() );
    ListViewMediaItem* item = dynamic_cast<ListViewMediaItem*>( listWidget->currentItem() );
    if ( item == NULL )
        return ;
    m_currentMedia = new InputMedia("file://" + item->fileInfo()->absoluteFilePath(), m_currentInstance );
    m_currentMedia->setupMedia();
    m_currentMedia->setDrawable( m_ui->clipRenderWidget->winId() );
    m_currentMedia->play();
}
