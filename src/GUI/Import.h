/*****************************************************************************
 * Import.h: Import menu
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Geoffroy Lacarriere <geoffroylaca@gmail.com>
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

#ifndef IMPORT_H
#define IMPORT_H

#include <QDialog>
#include "PreviewWidget.h"
#include "Media.h"
#include "TagWidget.h"
#include "ImportMediaCellView.h"

namespace Ui
{
    class Import;
}

class Import : public QDialog
{
    Q_OBJECT
    public:
        Import( QWidget *parent = 0 );
        ~Import();

    protected:
        void changeEvent( QEvent *e );

    private:
        Ui::Import*         m_ui;
        PreviewWidget*      m_previewWidget;
        TagWidget*          m_tagWidget;
        QHash<QUuid, Media*> m_mediaList;
        QUuid               m_currentUuid;

    private slots:
        void    accept();
        void    addMedia( Media* media, ImportMediaCellView* cell );
        void    removeMedia();
        void    setUIMetaData( QUuid uuid );
        void    clipViewRequested( QWidget* sender, QMouseEvent* ev );

    signals:
        void    mediaSelected( Media* );
};

#endif // IMPORT_H
