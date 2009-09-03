/*****************************************************************************
 * FileBrowser.cpp: FileBrowser Widget
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

#include "FileBrowser.h"
#include "Library.h"
#include <QStringList>
#include <QtDebug>

FileBrowser::FileBrowser( QWidget* parent ) : QWidget( parent )
{
    m_ui.setupUi( this );
    m_DirsModel = new QDirModel();
    m_FilesModel = new QDirModel();
    m_previousEntries = new QStack<QModelIndex>();
    m_forwadEntries = new QStack<QModelIndex>();

    QStringList filters;
    filters << "*.mp3" << "*.oga" << "*.flac" << "*.aac" << "*.wav";
    filters << "*.mov" << "*.avi" << "*.mkv" << "*.mpg" << "*.mpeg" << "*.wmv" << "*.mp4";
    filters << "*.gif" << "*.png" << "*.jpg";

    m_FilesModel->setFilter( QDir::AllDirs | QDir::Files | QDir::Readable | QDir::NoDotAndDotDot );
    m_FilesModel->sort( 2, Qt::AscendingOrder );
    m_FilesModel->sort( 0, Qt::AscendingOrder );
    m_FilesModel->setNameFilters( filters );
    m_ui.listViewBrowser->setModel( m_FilesModel );
    m_ui.listViewBrowser->setRootIndex( m_FilesModel->index( QDir::homePath() ) );

    m_DirsModel->setFilter( ( QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Readable ) );
    m_ui.treeViewBrowser->setModel( m_DirsModel );
    m_ui.treeViewBrowser->setRootIndex( m_DirsModel->index( QDir::rootPath() ) );
    m_ui.treeViewBrowser->setCurrentIndex( m_DirsModel->index( QDir::homePath() ) );
    m_ui.treeViewBrowser->setExpanded( m_ui.treeViewBrowser->currentIndex() , true );
    m_ui.treeViewBrowser->setColumnHidden( 1, true );
    m_ui.treeViewBrowser->setColumnHidden( 2, true );
    m_ui.treeViewBrowser->setColumnHidden( 3, true );
    m_ui.pushButtonBackward->setEnabled( false );
    m_ui.pushButtonForward->setEnabled( false );

    m_fsWatcher = new QFileSystemWatcher;
    m_fsWatcher->addPath( QDir::homePath() );
    m_currentlyWatchedDir = QDir::homePath();
    connect( m_fsWatcher, SIGNAL( directoryChanged(QString) ), m_DirsModel, SLOT( refresh() ) );
    connect( m_fsWatcher, SIGNAL( directoryChanged(QString) ), m_FilesModel, SLOT( refresh() ) );
}

FileBrowser::~FileBrowser()
{
    delete m_fsWatcher;
    delete m_DirsModel;
    delete m_FilesModel;
    delete m_forwadEntries;
    delete m_previousEntries;
}

void FileBrowser::addElementToHistory()
{
    m_previousEntries->push( m_DirsModel->index( m_FilesModel->filePath( m_ui.listViewBrowser->rootIndex() ) ) );
    if ( m_previousEntries->count() > 10 )
        m_previousEntries->pop_back();
    m_ui.pushButtonBackward->setEnabled( true );
}

void FileBrowser::ListViewBrowserDirectoryChanged( QModelIndex& index, bool history )
{
    if ( history == true )
        addElementToHistory();
    m_ui.listViewBrowser->setRootIndex( index );
    m_ui.treeViewBrowser->setCurrentIndex( m_DirsModel->index( m_FilesModel->filePath( index ) ) );
}

void FileBrowser::TreeViewBrowserDirectoryChanged( QModelIndex& index, bool history )
{
    if ( m_DirsModel->isDir( index ) )
    {
        updateFsWatcher( m_DirsModel->filePath( index ) );
        if ( history == true )
            addElementToHistory();
        m_ui.listViewBrowser->setRootIndex( m_FilesModel->index( m_DirsModel->filePath( index ) ) );
    }
}

void FileBrowser::on_treeViewBrowser_clicked( QModelIndex index )
{
    TreeViewBrowserDirectoryChanged( index );
    m_forwadEntries->clear();
    m_ui.pushButtonForward->setEnabled( false );
}

void FileBrowser::on_listViewBrowser_doubleClicked( QModelIndex index )
{
    if ( m_FilesModel->isDir( index ) )
    {
        updateFsWatcher( m_FilesModel->filePath( index ) );
        ListViewBrowserDirectoryChanged( index );
        m_forwadEntries->clear();
        m_ui.pushButtonForward->setEnabled( false );
    }
    else //TODO: this should be a signal.
        Library::getInstance()->newMediaLoadingAsked( m_FilesModel->filePath( index ) );
}

void FileBrowser::on_pushButtonBackward_clicked()
{
    if ( !m_previousEntries->isEmpty() )
    {
        m_forwadEntries->push( m_ui.treeViewBrowser->currentIndex() );
        TreeViewBrowserDirectoryChanged( m_previousEntries->first() , false );
        m_ui.treeViewBrowser->setCurrentIndex( m_previousEntries->pop() );
        m_ui.pushButtonForward->setEnabled( true );
        if ( m_previousEntries->isEmpty() )
            m_ui.pushButtonBackward->setEnabled( false );
    }
}

void FileBrowser::on_pushButtonForward_clicked()
{
    if ( !m_forwadEntries->isEmpty() )
    {
        m_previousEntries->push( m_ui.treeViewBrowser->currentIndex() );
        TreeViewBrowserDirectoryChanged( m_forwadEntries->first() , false );
        m_ui.treeViewBrowser->setCurrentIndex( m_forwadEntries->pop() );
        m_ui.pushButtonBackward->setEnabled( true );
        if ( m_forwadEntries->isEmpty() )
            m_ui.pushButtonForward->setEnabled( false );
    }
}

void FileBrowser::on_pushButtonParent_clicked()
{
    if ( !QDir(m_DirsModel->filePath( m_ui.treeViewBrowser->currentIndex() )).isRoot() )
    {
        addElementToHistory();
        updateFsWatcher( m_DirsModel->filePath( m_ui.treeViewBrowser->currentIndex().parent() ) );
        m_ui.listViewBrowser->setRootIndex( m_FilesModel->index( m_DirsModel->filePath( m_ui.treeViewBrowser->currentIndex().parent() ) ) );
        m_ui.treeViewBrowser->setCurrentIndex( m_DirsModel->index( m_FilesModel->filePath( m_ui.listViewBrowser->rootIndex() ) ) );
    }
}

void    FileBrowser::updateFsWatcher( const QString& newPath )
{
    m_fsWatcher->removePath( m_currentlyWatchedDir );
    m_currentlyWatchedDir = newPath;
    m_fsWatcher->addPath( newPath );
}
