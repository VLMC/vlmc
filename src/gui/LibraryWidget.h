/*****************************************************************************
 * LibraryWidget.h: Multimedia library GUI
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
#include <QSettings>
#include "Library.h"
#include "ListViewMediaItem.h"
#include "ui_LibraryWidget.h"

class LibraryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LibraryWidget( QWidget *parent = 0 );
    virtual ~LibraryWidget();

    ListViewMediaItem*                  addMedia( const Clip* clip, Library::FileType fileType );
    void                                removeMedia( const QUuid& uuid );
    int                                 getIndex( ListViewMediaItem* media );

protected:
    virtual void                        changeEvent( QEvent *e );

private:
    ListViewMediaItem*                  insertNewMedia( QString filename, Library::FileType fileType );
    void                                insertNewMediasFromFileDialog( QString title, QString filter, Library::FileType fileType );

    Ui::LibraryWidget                   m_ui;
    static QList<ListViewMediaItem*>*   m_medias;


public slots:
    void                                newClipLoaded( Clip* );
    void                                clipRemoved( const QUuid& );

private slots:
    void                                on_pushButtonAddMedia_clicked();
    void                                on_pushButtonRemoveMedia_clicked();

signals:
//    void    listViewMediaAdded(Clip* clip);
    void                                newClipLoadingAsked( const QString& );
    void                                removingClipAsked( const QUuid& );
};

#endif /* !LIBRARYWIDGET_H */
