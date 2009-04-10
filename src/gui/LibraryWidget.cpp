/*****************************************************************************
 * LibraryWidget.cpp: Multimedia library
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Clement CHAVANCE <chavance.c@gmail.com>
 *          Christophe Courtaut <christophe.courtaut@gmail.com>
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

#include <QDir>
#include "LibraryWidget.h"

QList<ListViewMediaItem*>* LibraryWidget::m_medias = NULL;

LibraryWidget::LibraryWidget( QWidget *parent ) : QWidget( parent )
{
    m_ui.setupUi( this );

    m_ui.listWidgetAudio->setType( ListViewMediaItem::Audio );
    m_ui.listWidgetImage->setType( ListViewMediaItem::Image );
    m_ui.listWidgetVideo->setType( ListViewMediaItem::Video );

    // *Always* force the selection of the first tab
    m_ui.LibraryTabs->setCurrentIndex( 0 );

    if ( LibraryWidget::m_medias == NULL )
        LibraryWidget::m_medias = new QList<ListViewMediaItem*>();
}

LibraryWidget::~LibraryWidget()
{
    if ( LibraryWidget::m_medias )
    {
        while ( !LibraryWidget::m_medias->isEmpty() )
            delete LibraryWidget::m_medias->takeLast();
        delete LibraryWidget::m_medias;
    }
}

ListViewMediaItem*  LibraryWidget::addMedia( QFileInfo* fileInfo, ListViewMediaItem::fType fileType )
{
    ListViewMediaItem* item = new ListViewMediaItem( fileInfo, fileType );
    emit listViewMediaAdded( item );
    m_medias->append( item );
    switch ( fileType )
    {
    case ListViewMediaItem::Audio:
        m_ui.listWidgetAudio->addItem( item );
        break;
    case ListViewMediaItem::Video:
        m_ui.listWidgetVideo->addItem( item );
        break;
    case ListViewMediaItem::Image:
        m_ui.listWidgetImage->addItem( item );
        break;
    }
    return item;
}

bool                LibraryWidget::removeMedia(ListViewMediaItem* item)
{
    if ( m_medias->contains( item ) )
    {
        switch( item->fileType() )
        {
        case ListViewMediaItem::Audio:
            this->m_ui.listWidgetAudio->removeItemWidget( item );
            break;
        case ListViewMediaItem::Image:
            this->m_ui.listWidgetImage->removeItemWidget( item );
            break;
        case ListViewMediaItem::Video:
            this->m_ui.listWidgetVideo->removeItemWidget( item );
            break;
        }
        bool ret = m_medias->removeOne( item );
        delete item;
        return ret;
    }
    return false;
}

ListViewMediaItem*                LibraryWidget::insertNewMedia( QString fileName, ListViewMediaItem::fType fileType )
{
    ListViewMediaItem* item = NULL;
    foreach( item, *m_medias )
    {
        if ( item->fileInfo()->absoluteFilePath() == fileName )
            return item;
    }
    QFileInfo* fileInfo = new QFileInfo( fileName );
    item = this->addMedia( fileInfo, fileType );
    return item;
}

void    LibraryWidget::insertNewMediasFromFileDialog( QString title, QString filter, ListViewMediaItem::fType filetype )
{
    QSettings settings;
    QString path = settings.value( "mediaLibraryDialogPath", QDir::homePath() ).toString();
    QStringList fileNames = QFileDialog::getOpenFileNames( this, title, path, filter );
    if ( fileNames.isEmpty() )
        return ;
    QString fileName;
    ListViewMediaItem* item = NULL;
    foreach ( fileName, fileNames )
        item = insertNewMedia( fileName, filetype );
    if ( item != NULL )
        settings.setValue( "mediaLibraryDialogPath" , item->fileInfo()->absoluteDir().absolutePath() );
    return ;
}

void LibraryWidget::on_pushButtonAddMedia_clicked()
{
    switch( m_ui.LibraryTabs->currentIndex() )
    {
    case 0:
        insertNewMediasFromFileDialog( tr( "Open Audios" ),
                                      tr( "Audio Files" ) + " (*.mp3 *.oga *.flac *.aac *.wav)" ,
                                      ListViewMediaItem::Audio );
        break;
    case 1:
        insertNewMediasFromFileDialog( tr( "Open Videos" ),
                                      tr( "Video Files" ) + " (*.mov *.avi *.mkv *.mpg *.mpeg *.wmv)" ,
                                      ListViewMediaItem::Video );
        break;
    case 2:
        insertNewMediasFromFileDialog( tr( "Open Images" ),
                                      tr( "Images Files" ) + " (*.gif *.png *.jpg)" ,
                                      ListViewMediaItem::Image );
        break;
    }
}


void LibraryWidget::on_pushButtonRemoveMedia_clicked()
{
    QListWidget* mediaList = ( QListWidget* )( this->m_ui.LibraryTabs->currentWidget()->children().back() );
    ListViewMediaItem* item = ( ListViewMediaItem* ) mediaList->currentItem();
    this->removeMedia( item );
}

void LibraryWidget::changeEvent( QEvent *e )
{
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        m_ui.retranslateUi( this );
        break;
    default:
        break;
    }
}
