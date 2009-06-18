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
#include <QUrl>
#include "LibraryWidget.h"

QList<ListViewMediaItem*>* LibraryWidget::m_medias = NULL;

LibraryWidget::LibraryWidget( QWidget *parent ) : QWidget( parent ), m_firstDirectoryBrowsing(true)
{
    m_ui.setupUi( this );

    m_ui.listWidgetAudio->setType( Media::Audio );
    m_ui.listWidgetImage->setType( Media::Image );
    m_ui.listWidgetVideo->setType( Media::Video );

    setAcceptDrops( true );

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

ListViewMediaItem*  LibraryWidget::addMedia( const Media* media, Media::FileType fileType )
{
    ListViewMediaItem* item = new ListViewMediaItem( media, fileType );
    //TODO: replace this :
    //emit listViewMediaAdded( item->getClip() );
    m_medias->append( item );
    switch ( fileType )
    {
    case Media::Audio:
        m_ui.listWidgetAudio->addItem( item );
        break;
    case Media::Video:
        m_ui.listWidgetVideo->addItem( item );
        break;
    case Media::Image:
        m_ui.listWidgetImage->addItem( item );
        break;
    }
    return item;
}

void                LibraryWidget::mediaRemoved( const QUuid& uuid )
{
    removeMedia( uuid );
}

void                LibraryWidget::removeMedia( const QUuid& uuid )
{
    ListViewMediaItem* item;
    foreach (item, *m_medias)
    {
        if ( item->getMedia()->getUuid() == uuid )
        {
            switch( item->getFileType() )
            {
            case Media::Audio:
                this->m_ui.listWidgetAudio->removeItemWidget( item );
                break;
            case Media::Image:
                this->m_ui.listWidgetImage->removeItemWidget( item );
                break;
            case Media::Video:
                this->m_ui.listWidgetVideo->removeItemWidget( item );
                break;
            }
            m_medias->removeOne( item );
            delete item;
        }
    }
}
//
//ListViewMediaItem*                LibraryWidget::insertNewMedia( QString fileName, ListViewMediaItem::fType fileType )
//{
//    ListViewMediaItem* item = NULL;
//    foreach( item, *m_medias )
//    {
//        if ( item->getClip()->getFileInfo()->absoluteFilePath() == fileName )
//            return item;
//    }
//    QFileInfo* fileInfo = new QFileInfo( fileName );
//    item = addMedia( fileInfo, fileType );
//    delete fileInfo;
//    return item;
//}

void    LibraryWidget::newMediaLoaded( Media* media )
{
    //From here, the clip is const.
    addMedia( media, Media::Video );
    m_ui.LibraryTabs->setCurrentIndex( 1 );
}

void    LibraryWidget::insertNewMediasFromFileDialog( QString title, QString filter, Media::FileType fileType )
{
    QSettings settings;
    QString path;

    if ( m_firstDirectoryBrowsing == true )
         path = settings.value( "mediaLibraryDialogPath", QDir::homePath() ).toString();
    else
         path = m_lastDirectoryBrowsed;

    QStringList fileNames = QFileDialog::getOpenFileNames( this, title, path, filter );
    if ( fileNames.isEmpty() )
        return ;
    m_lastDirectoryBrowsed = QFileInfo( fileNames.front() ).absolutePath();
    m_firstDirectoryBrowsing = false;
    QString filePath;
//    ListViewMediaItem* item = NULL;
    foreach ( filePath, fileNames )
    {
//        item = insertNewMedia( fileName, filetype );
//        if( fileType == Media::Video )
            emit newMediaLoadingAsked( filePath );
    }
//    if ( item != NULL )
//        settings.setValue( "mediaLibraryDialogPath" , item->getClip()->getFileInfo()->absoluteDir().absolutePath() );
//    return ;
}

void LibraryWidget::on_pushButtonAddMedia_clicked()
{
    switch( m_ui.LibraryTabs->currentIndex() )
    {
    case 0:
        insertNewMediasFromFileDialog( tr( "Open Audios" ),
                                      tr( "Audio Files" ) + " (*.mp3 *.oga *.flac *.aac *.wav)" ,
                                      Media::Audio );
        break;
    case 1:
        insertNewMediasFromFileDialog( tr( "Open Videos" ),
                                      tr( "Video Files" ) + " (*.mov *.avi *.mkv *.mpg *.mpeg *.wmv *.mp4)" ,
                                      Media::Video );
        break;
    case 2:
        insertNewMediasFromFileDialog( tr( "Open Images" ),
                                      tr( "Images Files" ) + " (*.gif *.png *.jpg)" ,
                                      Media::Image );
        break;
    }
}


void LibraryWidget::on_pushButtonRemoveMedia_clicked()
{
    QListWidget* mediaList = ( QListWidget* )( m_ui.LibraryTabs->currentWidget()->children().back() );
    ListViewMediaItem* item = ( ListViewMediaItem* ) mediaList->currentItem();
    if ( !item ) return;
//    removeMedia( item );
    emit removingMediaAsked( item->getMedia()->getUuid() );
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

void    LibraryWidget::dragEnterEvent( QDragEnterEvent* event )
{
    if ( event->mimeData()->urls().count() >= 1 )
        event->acceptProposedAction();
}

void    LibraryWidget::dropEvent( QDropEvent* event )
{
    QUrl url;
    Library* lib = Library::getInstance();
    foreach ( url, event->mimeData()->urls() )
    {
        lib->newMediaLoadingAsked( url.path() );
    }
}
