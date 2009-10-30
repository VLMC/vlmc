/*****************************************************************************
 * ImportBrowser.h: ImportBrowser Widget
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

#ifndef IMPORTBROWSER_H
#define IMPORTBROWSER_H

#include <QWidget>
#include <QDirModel>
#include <QModelIndexList>
#include <QListWidget>
#include <QStack>
#include <QFileSystemWatcher>
#include "ui_ImportBrowser.h"
#include <QStringListModel>
#include "StackViewController.h"
#include "ImportMediaListController.h"
#include "MetaDataWorker.h"

class ImportBrowser : public QWidget
{
    Q_OBJECT
public:    
    ImportBrowser( QWidget* parent = NULL );
    virtual ~ImportBrowser();
    ImportMediaListController* getMediaListView() { return m_mediaList; }
    ImportMediaListController* getClipListView() { return m_clipList; }
    StackViewController*       getStackViewController() { return m_nav; }

private:
    void    TreeViewBrowserDirectoryChanged( QModelIndex& index );
    void    updateFsWatcher( const QString& newPath );

private:
    Ui::ImportBrowser           m_ui;
    QDirModel*                  m_FilesModel;
    QFileSystemWatcher*         m_fsWatcher;
    QString                     m_currentlyWatchedDir;
    MetaDataWorker*             m_metaDataWorker;
    StackViewController*        m_nav;
    ImportMediaListController*  m_mediaList;
    ImportMediaListController*  m_clipList;

private slots:
    void on_pushButtonForward_clicked();
    void on_pushButtonBackward_clicked();
    void on_treeViewBrowser_clicked( QModelIndex index );
    void on_treeViewBrowser_doubleClicked( QModelIndex index);

signals:
    void    mediaSelected( QFileInfo fileInfos );
    void    mediaAdded( Media* media, ImportMediaCellView* );
    void    mediaRemoved();
};

#endif /* !IMPORTBROWSER_H */
