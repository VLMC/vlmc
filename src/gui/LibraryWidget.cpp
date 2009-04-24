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

    m_ui.listWidgetAudio->setType( Library::Audio );
    m_ui.listWidgetImage->setType( Library::Image );
    m_ui.listWidgetVideo->setType( Library::Video );

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

ListViewMediaItem*  LibraryWidget::addMedia( const Clip* clip, Library::FileType fileType )
{
    ListViewMediaItem* item = new ListViewMediaItem( clip, fileType );
    //TODO: replace this :
    //emit listViewMediaAdded( item->getClip() );
    m_medias->append( item );
    switch ( fileType )
    {
    case Library::Audio:
        m_ui.listWidgetAudio->addItem( item );
        break;
    case Library::Video:
        m_ui.listWidgetVideo->addItem( item );
        break;
    case Library::Image:
        m_ui.listWidgetImage->addItem( item );
        break;
    }
    return item;
}

void                LibraryWidget::clipRemoved( const QUuid& uuid )
{
    removeMedia( uuid );
}

void                LibraryWidget::removeMedia( const QUuid& uuid )
{
    ListViewMediaItem* item;
    foreach (item, *m_medias)
    {
        if ( item->getClip()->getUuid() == uuid )
        {
            switch( item->getFileType() )
            {
            case Library::Audio:
                this->m_ui.listWidgetAudio->removeItemWidget( item );
                break;
            case Library::Image:
                this->m_ui.listWidgetImage->removeItemWidget( item );
                break;
            case Library::Video:
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

void    LibraryWidget::newClipLoaded( Clip* clip )
{
    //From here, the clip is const.
    addMedia( clip, Library::Video );
}

void    LibraryWidget::insertNewMediasFromFileDialog( QString title, QString filter, Library::FileType fileType )
{
    QSettings settings;
    QString path = settings.value( "mediaLibraryDialogPath", QDir::homePath() ).toString();
    QStringList fileNames = QFileDialog::getOpenFileNames( this, title, path, filter );
    if ( fileNames.isEmpty() )
        return ;
    QString filePath;
//    ListViewMediaItem* item = NULL;
    foreach ( filePath, fileNames )
    {
//        item = insertNewMedia( fileName, filetype );
//        if( fileType == Library::Video )
            emit newClipLoadingAsked( filePath );
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
                                      Library::Audio );
        break;
    case 1:
        insertNewMediasFromFileDialog( tr( "Open Videos" ),
                                      tr( "Video Files" ) + " (*.mov *.avi *.mkv *.mpg *.mpeg *.wmv *.mp4)" ,
                                      Library::Video );
        break;
    case 2:
        insertNewMediasFromFileDialog( tr( "Open Images" ),
                                      tr( "Images Files" ) + " (*.gif *.png *.jpg)" ,
                                      Library::Image );
        break;
    }
}


void LibraryWidget::on_pushButtonRemoveMedia_clicked()
{
    QListWidget* mediaList = ( QListWidget* )( m_ui.LibraryTabs->currentWidget()->children().back() );
    ListViewMediaItem* item = ( ListViewMediaItem* ) mediaList->currentItem();
    if ( !item ) return;
//    removeMedia( item );
    emit removingClipAsked( item->getClip()->getUuid() );
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
