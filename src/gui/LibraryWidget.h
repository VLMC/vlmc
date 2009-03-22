/*****************************************************************************
 * LibraryWidget.h: VLMC library widget header
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

#ifndef LIBRARYWIDGET_H
#define LIBRARYWIDGET_H

#include <QTabWidget>
#include <QListWidget>
#include <QFileInfoList>
#include <QMessageBox>
#include <QFileDialog>
#include <QtDebug>
#include <QMimeData>
#include <QDrag>
#include <QList>
#include "ui_LibraryWidget.h"

class ListViewMediaItem : public QListWidgetItem
{
public:
    enum fType
    {
        Audio,
        Video,
        Image
    };

    ListViewMediaItem( QFileInfo* fileInfo, ListViewMediaItem::fType fType, QListWidget* parent = 0, int type = Type );
    virtual ~ListViewMediaItem();
    QFileInfo* fileInfo;
    ListViewMediaItem::fType fileType;
protected:
    void    mousePressEvent( QMouseEvent* event );
};

class LibraryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LibraryWidget( QWidget *parent = 0 );

    ListViewMediaItem*  addMedia( QFileInfo* fileInfo, ListViewMediaItem::fType fileType );
    bool                removeMedia( ListViewMediaItem* item );
    int                 getIndex( ListViewMediaItem* media );
private:
    QString             getNewMediaFileName( QString title, QString filter, ListViewMediaItem::fType );
    ListViewMediaItem*                insertNewMediaFromFileDialog(QString title, QString filter, ListViewMediaItem::fType fileType);
    Ui::LibraryWidget                   m_ui;
    static QList<ListViewMediaItem*>* m_medias;

private slots:
    void on_pushButtonAddMedia_clicked();
    void on_pushButtonRemoveMedia_clicked();
};


#endif /* !LIBRARYWIDGET_H */
