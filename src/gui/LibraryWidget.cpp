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

LibraryWidget::LibraryWidget( QWidget *parent ) : QWidget( parent ), m_firstDirectoryBrowsing(true)
{
    m_ui.setupUi( this );

    m_ui.listWidgetAudio->setType( Media::Audio );
    m_ui.listWidgetImage->setType( Media::Image );
    m_ui.listWidgetVideo->setType( Media::Video );

    setAcceptDrops( true );

    // *Always* force the selection of the first tab
    m_ui.LibraryTabs->setCurrentIndex( 0 );

    m_clips = new QList<ListViewMediaItem*>;
    connect( m_ui.filterInput, SIGNAL( textChanged( QString ) ), this, SLOT( updateFilter( QString ) ) );
    connect( m_ui.clearFilterButton, SIGNAL( clicked() ), m_ui.filterInput, SLOT( clear() ) );
    connect( m_ui.LibraryTabs, SIGNAL( currentChanged( int ) ), this, SLOT( tabChanged( int ) ) );
}

LibraryWidget::~LibraryWidget()
{
    while ( m_clips->isEmpty() == false )
        delete m_clips->takeLast();
    delete m_clips;
}

ListViewMediaItem*  LibraryWidget::addClip( Clip* clip )
{
    Media::FileType fileType = clip->getParent()->getFileType();
    ListViewMediaItem* item = new ListViewMediaItem( clip, fileType );

    m_clips->append( item );
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
    foreach (item, *m_clips)
    {
        if ( item->getClip()->getUuid() == uuid )
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
            m_clips->removeOne( item );
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
    addClip( clip );
    m_ui.LibraryTabs->setCurrentIndex( (int) clip->getParent()->getFileType() );
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
                                      tr( "Audio Files" ) + " (" + Media::AudioExtensions + ")" ,
                                      Media::Audio );
        break;
    case 1:
        insertNewMediasFromFileDialog( tr( "Open Videos" ),
                                      tr( "Video Files" ) + " (" + Media::VideoExtensions + ")" ,
                                      Media::Video );
        break;
    case 2:
        insertNewMediasFromFileDialog( tr( "Open Images" ),
                                      tr( "Images Files" ) + " (" + Media::ImageExtensions + ")" ,
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
    emit removingMediaAsked( item->getClip()->getParent()->getUuid() );
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

const MediaListWidget*      LibraryWidget::getVideoListWidget() const
{
    return m_ui.listWidgetVideo;
}

const MediaListWidget*      LibraryWidget::getAudioListWidget() const
{
    return m_ui.listWidgetAudio;
}

void                        LibraryWidget::updateFilter( const QString& filter )
{
    QListWidget* mediaList = ( QListWidget* )( m_ui.LibraryTabs->currentWidget()->children().back() );
    unsigned int count = mediaList->count();
    for ( unsigned int i = 0; i < count; ++i )
    {
        ListViewMediaItem*  item = static_cast<ListViewMediaItem*>( mediaList->item( i ) );
        if ( item != NULL )
        {
            item->setHidden( !(item->getClip()->matchMetaTag( filter )) );
        }
    }
}

void                        LibraryWidget::tabChanged( int )
{
    updateFilter( m_ui.filterInput->text() );
}
