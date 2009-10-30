/*****************************************************************************
 * ImportBrowser.cpp: ImportBrowser Widget
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Christophe Courtaut <christophe.courtaut@gmail.com>
 *          Geoffroy Lacarriere <geoffroylaca@gmail.com>
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

#include "ImportBrowser.h"
#include "Library.h"
#include <QStringList>
#include <QtDebug>
#include <QStringListModel>
#include "FileInfoListModel.h"

ImportBrowser::ImportBrowser( QWidget* parent ) : QWidget( parent )
{
    m_ui.setupUi( this );
    m_FilesModel = new QDirModel();
    m_nav = new StackViewController( m_ui.MediaListWidget, false );
    m_mediaList = new ImportMediaListController( m_nav );
    m_clipList = new ImportMediaListController( m_nav );
    m_nav->pushViewController( m_mediaList );
    

    QStringList filters;
    //Video
    filters << "*.mov" << "*.avi" << "*.mkv" << "*.mpg" << "*.mpeg" << "*.wmv" << "*.mp4";
    //Audio
    filters << "*.mp3" << "*.oga" << "*.flac" << "*.aac" << "*.wav";
    //Picture
    filters << "*.gif" << "*.png" << "*.jpg";

    m_FilesModel->setFilter( QDir::AllDirs | QDir::Files | QDir::Readable | QDir::NoDotAndDotDot );
    m_FilesModel->sort( 2, Qt::AscendingOrder );
    m_FilesModel->sort( 0, Qt::AscendingOrder );
    m_FilesModel->setNameFilters( filters );

    m_ui.treeViewBrowser->setModel( m_FilesModel );
    m_ui.treeViewBrowser->setRootIndex( m_FilesModel->index( QDir::rootPath() ) );
    m_ui.treeViewBrowser->setCurrentIndex( m_FilesModel->index( QDir::homePath() ) );
    m_ui.treeViewBrowser->setExpanded( m_ui.treeViewBrowser->currentIndex() , true );
    m_ui.treeViewBrowser->setColumnHidden( 1, true );
    m_ui.treeViewBrowser->setColumnHidden( 2, true );
    m_ui.treeViewBrowser->setColumnHidden( 3, true );
    m_ui.pushButtonBackward->setEnabled( false );
    m_ui.pushButtonForward->setEnabled( false );

    m_fsWatcher = new QFileSystemWatcher;
    m_fsWatcher->addPath( QDir::homePath() );
    m_currentlyWatchedDir = QDir::homePath();
    connect( m_fsWatcher, SIGNAL( directoryChanged( QString ) ), m_FilesModel, SLOT( refresh() ) );
}

ImportBrowser::~ImportBrowser()
{
    delete m_fsWatcher;
    delete m_FilesModel;
}

void ImportBrowser::TreeViewBrowserDirectoryChanged( QModelIndex& index )
{
    if ( m_FilesModel->isDir( index ) )
    {
        updateFsWatcher( m_FilesModel->filePath( index ) );
    }
}

void ImportBrowser::on_treeViewBrowser_clicked( QModelIndex index )
{
    TreeViewBrowserDirectoryChanged( index );
    m_ui.pushButtonForward->setEnabled( true );
}

void ImportBrowser::on_treeViewBrowser_doubleClicked( QModelIndex index)
{
    if ( !m_FilesModel->isDir( index ) )
        on_pushButtonForward_clicked();
}

void ImportBrowser::on_pushButtonBackward_clicked()
{
    emit mediaRemoved();
    if ( m_mediaList->getMediaCellList()->isEmpty() )
        m_ui.pushButtonBackward->setEnabled( false );
}

void ImportBrowser::on_pushButtonForward_clicked()
{
    if ( !m_FilesModel->isDir( m_ui.treeViewBrowser->selectionModel()->currentIndex() ) )
    {
        Media* media = new Media( m_FilesModel->fileInfo( m_ui.treeViewBrowser->selectionModel()->currentIndex() ).filePath() );
        if ( !m_mediaList->getMediaCellList()->contains( media->getUuid() ) )
        {
            m_mediaList->addMedia( media );
            emit mediaAdded( media, m_mediaList->getCell( media->getUuid() ) );

            connect( media, SIGNAL( metaDataComputed( Media* ) ), m_mediaList, SLOT( metaDataComputed( Media* ) ), Qt::DirectConnection );
            m_metaDataWorker = new MetaDataWorker( media );
            m_metaDataWorker->start();
        }
        else
            delete media;
    }
    else
        for( int i = 0; i < m_FilesModel->rowCount( m_ui.treeViewBrowser->selectionModel()->currentIndex() ); i++)
            {
                QFileInfo info = m_FilesModel->fileInfo( m_ui.treeViewBrowser->selectionModel()->currentIndex().child( i, 0 ) );
                if ( info.isDir() )
                    continue ;
                Media* media = new Media( info.filePath() );
                if ( !m_mediaList->getMediaCellList()->contains( media->getUuid() ) )
                {
                    m_mediaList->addMedia( media );
                    emit mediaAdded( media, m_mediaList->getCell( media->getUuid() ) );

                    connect( media, SIGNAL( metaDataComputed( Media* ) ), m_mediaList, SLOT( metaDataComputed( Media* ) ), Qt::DirectConnection );
                    m_metaDataWorker = new MetaDataWorker( media );
                    m_metaDataWorker->start();
                }
                else
                    delete media;
            }
    m_ui.pushButtonBackward->setEnabled( true );
}

void    ImportBrowser::updateFsWatcher( const QString& newPath )
{
    m_fsWatcher->removePath( m_currentlyWatchedDir );
    m_currentlyWatchedDir = newPath;
    m_fsWatcher->addPath( newPath );
}
