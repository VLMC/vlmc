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
    m_ImportListModel = new FileInfoListModel();

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

    m_ui.listViewBrowser->setModel( m_ImportListModel );
    m_ui.listViewBrowser->setRootIndex( QModelIndex() );
    m_ui.listViewBrowser->setMouseTracking( false );

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
    delete m_ImportListModel;
}

void ImportBrowser::TreeViewBrowserDirectoryChanged( QModelIndex& index )
{
    if ( m_FilesModel->isDir( index ) )
    {
        updateFsWatcher( m_FilesModel->filePath( index ) );
        m_ui.listViewBrowser->setRootIndex( m_FilesModel->index( m_FilesModel->filePath( index ) ) );
    }
}

void ImportBrowser::on_listViewBrowser_clicked( QModelIndex index )
{
    FileInfoListModel* model = static_cast<FileInfoListModel*>( m_ImportListModel );
    emit mediaSelected( model->fileInfo( index ) );
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
    FileInfoListModel* model = static_cast<FileInfoListModel*>( m_ImportListModel );
    m_mediaInfoList.removeOne( model->fileInfo( m_ui.listViewBrowser->selectionModel()->currentIndex() ) );
    emit mediaRemoved( model->fileInfo( m_ui.listViewBrowser->selectionModel()->currentIndex() ) );
    model->setFileInfoList( m_mediaInfoList );

    if ( m_mediaInfoList.isEmpty() )
        m_ui.pushButtonBackward->setEnabled( false );
}

void ImportBrowser::on_pushButtonForward_clicked()
{
    FileInfoListModel* model = static_cast<FileInfoListModel*>( m_ImportListModel );

    if ( !m_FilesModel->isDir( m_ui.treeViewBrowser->selectionModel()->currentIndex() ) &&
        !m_mediaInfoList.contains( m_FilesModel->fileInfo( m_ui.treeViewBrowser->selectionModel()->currentIndex() ) ) )
    {
        m_mediaInfoList << m_FilesModel->fileInfo( m_ui.treeViewBrowser->selectionModel()->currentIndex() );
        emit mediaAdded( m_FilesModel->fileInfo( m_ui.treeViewBrowser->selectionModel()->currentIndex() ) );
    }
    else
        for( int i = 0; i < m_FilesModel->rowCount( m_ui.treeViewBrowser->selectionModel()->currentIndex() ); i++)
            if ( !m_mediaInfoList.contains( m_FilesModel->fileInfo( m_ui.treeViewBrowser->selectionModel()->currentIndex().child( i, 0 ) ) ) )
            {
                m_mediaInfoList << m_FilesModel->fileInfo( m_ui.treeViewBrowser->selectionModel()->currentIndex().child( i, 0 ) );
                emit mediaAdded( m_FilesModel->fileInfo( m_ui.treeViewBrowser->selectionModel()->currentIndex().child( i, 0 ) ) );
            }
    model->setFileInfoList( m_mediaInfoList );
    m_ui.pushButtonBackward->setEnabled( true );
}

void    ImportBrowser::updateFsWatcher( const QString& newPath )
{
    m_fsWatcher->removePath( m_currentlyWatchedDir );
    m_currentlyWatchedDir = newPath;
    m_fsWatcher->addPath( newPath );
}
