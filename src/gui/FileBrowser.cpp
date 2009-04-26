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
#include <QStringList>
#include <QtDebug>

FileBrowser::FileBrowser( QWidget* parent ) : QWidget( parent )
{
    m_ui.setupUi( this );
    m_DirsModel = new QDirModel();
    m_FilesModel = new QDirModel();
    QStringList filters;
    filters << "*.mp3" << "*.oga" << "*.flac" << "*.aac" << "*.wav";
    filters << "*.mov" << "*.avi" << "*.mkv" << "*.mpg" << "*.mpeg" << "*.wmv" << "*.mp4";
    filters << "*.gif" << "*.png" << "*.jpg";

    m_FilesModel->setFilter( QDir::Dirs | QDir::Files | QDir::Readable | QDir::NoDotAndDotDot );
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

    //m_ui.listViewBrowser->setSelectionModel( m_ui.treeViewBrowser->selectionModel() );
}

void FileBrowser::on_treeViewBrowser_clicked( QModelIndex index )
{
    if ( m_DirsModel->isDir( index ) )
    {
        m_ui.listViewBrowser->setRootIndex( m_FilesModel->index( m_DirsModel->filePath( index ) ) );
    }
}

void FileBrowser::on_listViewBrowser_doubleClicked(QModelIndex index)
{
    if ( m_FilesModel->isDir( index ) )
    {
        m_ui.listViewBrowser->setRootIndex( index );
        m_ui.treeViewBrowser->setCurrentIndex( m_DirsModel->index( m_FilesModel->filePath( index ) ) );
    }
}
