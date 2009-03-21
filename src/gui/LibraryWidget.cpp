/*****************************************************************************
 * LibraryWidget.cpp: VLMC LibraryWidget
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Ludovic Fauvet <etix@l0cal.com>
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

#include "LibraryWidget.h"

LibraryWidget::LibraryWidget( QWidget *parent ) : QWidget( parent )
{
    m_ui.setupUi( this );
}

void LibraryWidget::on_pushButtonAddMedia_clicked()
{
    QFileInfo* fileInfo = NULL;
    QListWidgetItem* item;
    qDebug() << m_ui.LibraryTabs->currentIndex();
    switch( m_ui.LibraryTabs->currentIndex() )
    {
    case 0:
        fileInfo = new QFileInfo( QFileDialog::getOpenFileName( this, tr( "Open Audios" ), "/home/kri5", tr( "Audio Files (*.mp3 *.oga *.flac *.aac *.wav)" )) );
        m_ui.listWidgetAudio->addItem(new ListViewMediaItem( fileInfo ));
        break;
    case 1:
        fileInfo = new QFileInfo( QFileDialog::getOpenFileName( this, tr( "Open Videos" ), "/home/kri5", tr( "Video Files (*.mov *.avi *.mkv)" )) );
        m_ui.listWidgetVideo->addItem(new ListViewMediaItem( fileInfo ));
        break;
    case 2:
        fileInfo = new QFileInfo( QFileDialog::getOpenFileName( this, tr( "Open Images" ), "/home/kri5", tr( "Video Files (*.gif *.png *.jpg)" )) );
        m_ui.listWidgetImage->addItem(new ListViewMediaItem( fileInfo ));
        break;
    default:
        break;
    }
    m_ui.listWidgetAll->addItem(new ListViewMediaItem( fileInfo ));
}

void LibraryWidget::on_LibraryTabs_currentChanged(int index)
{
    if (index == 3)
    {
        m_ui.pushButtonAddMedia->setEnabled(false);
        m_ui.pushButtonRemoveMedia->setEnabled(false);
    }
    else
    {
        m_ui.pushButtonAddMedia->setEnabled(true);
        m_ui.pushButtonRemoveMedia->setEnabled(true);
    }
}

ListViewMediaItem::ListViewMediaItem( QFileInfo* fInfo, QListWidget* parent, int type ) : QListWidgetItem( parent, type )
{
    fileInfo = fInfo;
    setText(fileInfo->baseName());
}
