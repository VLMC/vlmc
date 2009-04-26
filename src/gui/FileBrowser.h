/*****************************************************************************
 * FileBrowser.h: FileBrowser Widget
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

#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QWidget>
#include <QDirModel>
#include "ui_FileBrowser.h"

class FileBrowser : public QWidget
{
    Q_OBJECT
public:
    FileBrowser( QWidget* parent = NULL );
private:
    Ui::FileBrowser m_ui;
    QDirModel*      m_DirsModel;
    QDirModel*      m_FilesModel;

private slots:
    void on_listViewBrowser_doubleClicked(QModelIndex index);
    void on_treeViewBrowser_clicked( QModelIndex index );
};

#endif /* !FILEBROWSER_H */
