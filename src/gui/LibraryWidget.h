/*****************************************************************************
 * LibraryWidget.h: Multimedia library
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
#include <QHash>
#include <QUuid>
#include <QSettings>
#include "ListViewMediaItem.h"
#include "ui_LibraryWidget.h"
#include "Clip.h"
#include "Singleton.hpp"

class LibraryWidget : public QWidget//, public Singleton<LibraryWidget>
{
    Q_OBJECT

public:
    ListViewMediaItem*  addMedia( QFileInfo* fileInfo, ListViewMediaItem::fType fileType );
    bool                removeMedia( ListViewMediaItem* item );
    int                 getIndex( ListViewMediaItem* media );
    Clip*               getClip( const QUuid& uuid );

    //Temporary !!!
    static LibraryWidget*      getInstance( QWidget* parent = NULL )
    {
        if ( m_instance == NULL )
            m_instance = new LibraryWidget( parent );
        return m_instance;
    }

protected:
    explicit LibraryWidget( QWidget *parent = 0 );
    virtual ~LibraryWidget();
    virtual void changeEvent( QEvent *e );

private:
    ListViewMediaItem*  insertNewMedia( QString filename, ListViewMediaItem::fType fileType );
    void                insertNewMediasFromFileDialog( QString title, QString filter, ListViewMediaItem::fType fileType );

    Ui::LibraryWidget                   m_ui;
    static QList<ListViewMediaItem*>*   m_medias;
    QHash<QUuid, Clip*>                 m_mediaHash;

    //TODO: remove the singleton from this class...
    static  LibraryWidget*  m_instance;
    //friend class    Singleton<LibraryWidget>;

private slots:
    void on_pushButtonAddMedia_clicked();
    void on_pushButtonRemoveMedia_clicked();

signals:
    void    listViewMediaAdded(Clip* clip);
};

#endif /* !LIBRARYWIDGET_H */
