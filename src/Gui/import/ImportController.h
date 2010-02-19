/*****************************************************************************
 * ImportController.h
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
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

/** \file
 * This file ImportController contains class declaration/definition.
 * It's the the controler of the import menu widget of vlmc.
 */

#ifndef IMPORTCONTROLLER_H
#define IMPORTCONTROLLER_H

#include "Clip.h"
#include "ImportMediaListController.h"
#include "Media.h"
#include "PreviewWidget.h"
#include "StackViewController.h"
#include "TagWidget.h"

#include <QDialog>
#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QProgressDialog>

namespace Ui
{
    class ImportController;
}

/**
 *  \class ImportController
 *  \brief Controller of the import menu
 */
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
        void                        saveCurrentPath();
        void                        restoreCurrentPath();
        void                        collapseAllButCurrentPath();
        void                        deleteTemporaryMedias();
        void                        importMedia( const QString &filePath );
        void                        importDir( const QString &path );
        Ui::ImportController*       m_ui;
        PreviewWidget*              m_preview;
        StackViewController*        m_stackNav;
        TagWidget*                  m_tag;
        ImportMediaListController*  m_mediaListController;
        ImportMediaListController*  m_clipListController;
        QFileSystemModel            *m_filesModel;
        QString                     m_currentlyWatchedDir;
        QUuid                       m_currentUuid;
        QUuid                       m_savedUuid;
        bool                        m_controllerSwitched;
        QHash< QUuid, Media*>       m_temporaryMedias;
        quint32                     m_nbMediaToLoad;
        quint32                     m_nbMediaLoaded;

    public slots:
        void        updateMediaRequested( const Media* media );
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
        void        mediaLoaded();
        void        failedToLoad( Media* media );
        void        hideErrors();

    signals:
        void        mediaSelected( Media* );
        void        clipSelected( Clip* );
};

#endif // IMPORTCONTROLLER_H
