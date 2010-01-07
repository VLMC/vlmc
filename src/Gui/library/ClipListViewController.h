/*****************************************************************************
 * ClipListViewController.h: controller for the library clip list
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Clement CHAVANCE <chavance.c@gmail.com>
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

#ifndef CLIPLISTVIEWCONTROLLER_H
#define CLIPLISTVIEWCONTROLLER_H

#include "ListViewController.h"
#include "Media.h"
#include "Clip.h"

class ClipListViewController : public ListViewController
{
    Q_OBJECT
    public:
        explicit ClipListViewController( StackViewController* nav, const QUuid& uuid);
        ~ClipListViewController();

        void    addClipsFromMedia( Media* media );
        int     getNbDeletion() const;

    private:
        void    addClip( Clip* clip );
        void    clean();
        StackViewController*    m_nav;
        QHash<QUuid, QWidget*>  m_cells;
        QUuid                   m_mediaId;
        QUuid                   m_currentUuid;
        int                     m_deletion;

    public slots:
        void    cellSelection( const QUuid& uuid );
        void    clipDeletion( const QUuid& uuid );

    signals:
        void    clipSelected( const QUuid& uuid );
        void    clipDeleted( const QUuid& uuid );
};

#endif
