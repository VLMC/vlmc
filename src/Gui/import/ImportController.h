/*****************************************************************************
 * ImportController.h
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Geoffroy Lacarriere <geoffroylaca@gmail.com>
 *          Thomas Boquet <thomas.boquet@gmail.com>
 *          Clement Chavance <chavance.c@gmail.com>
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

#ifndef IMPORTCONTROLLER_H
#define IMPORTCONTROLLER_H

#include <QDialog>
#include <QDirModel>
#include <QFileSystemWatcher>

#include "Media.h"
#include "Clip.h"
#include "ImportModel.h"
#include "PreviewWidget.h"
#include "StackViewController.h"
#include "TagWidget.h"
#include "ImportMediaListController.h"

namespace Ui
{
    class ImportController;
}

class ImportController : public QDialog
{
    Q_OBJECT

    public:
        ImportController(QWidget *parent = 0);
        ~ImportController();
        void    setUIMetaData( Media* media );
        void    setUIMetaData( Clip* clip );

    protected:
        void changeEvent( QEvent *e );

    private:
        Ui::ImportController*       m_ui;
        PreviewWidget*              m_preview;
        ImportModel*                m_model;
        StackViewController*        m_stackNav;
        TagWidget*                  m_tag;
        ImportMediaListController*  m_mediaListController;
        ImportMediaListController*  m_clipListController;
        QDirModel*                  m_filesModel;
        QFileSystemWatcher*         m_fsWatcher;
        QString                     m_currentlyWatchedDir;
        QUuid                       m_currentUuid;
        QUuid                       m_savedUuid;
        bool                        m_controllerSwitched;

    public slots:
        void        newMediaLoaded( Media* media );
        void        updateMediaRequested( Media* media );
        void        accept();
        void        reject();
        void        mediaSelection( const QUuid& uuid );
        void        clipSelection( const QUuid& uuid );
        void        mediaDeletion( const QUuid& uuid );
        void        clipDeletion( const QUuid& uuid );
        void        showClipList( const QUuid& uuid );
        void        restoreContext();

    private slots:
        void        forwardButtonClicked();
        void        treeViewClicked( const QModelIndex& index );
        void        treeViewDoubleClicked( const QModelIndex& index );

    signals:
        void        mediaSelected( Media* );
        void        clipSelected( Clip* );
};

#endif // IMPORTCONTROLLER_H
