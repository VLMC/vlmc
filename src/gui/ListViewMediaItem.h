/*****************************************************************************
 * ListViewMediaItem.h: VLMC library widget header
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

#ifndef LISTVIEWMEDIAITEM_H
#define LISTVIEWMEDIAITEM_H

#include <QObject>
#include <QFileInfo>
#include <QListWidgetItem>
#include "Clip.h"

/**
 *  \class ListViewMediaItem
 *  \brief Items class to use inside MediaListView widget
 */

class ListViewMediaItem : public QObject, public QListWidgetItem
{
    Q_OBJECT

public:
    /**
     *  \enum fType
     *  \brief enum to determine file type
     */
    enum fType
    {
        Audio,
        Video,
        Image
    };

    /**
     * \brief Constructor
     *
     * \param fileInfo the QFileInfo refering to the file
     * \param fType the type of the media
     * \param parent parent widget
     * \param type
     */
    ListViewMediaItem( QFileInfo* fileInfo, ListViewMediaItem::fType fType, QListWidget* parent = 0, int type = Type );

    /**
     *  \brief Destructor
     */
    virtual ~ListViewMediaItem();

    /**
     * \brief Getter for filetype
     * \return filetype of the item
     */
    ListViewMediaItem::fType fileType() { return m_fileType; }

    /**
      * \brief Get the current media snapshot
      * \return the current media snapshot as a QPixmap*, or NULL if there is no current media.
      */
    //const QPixmap*            getSnapshot() const;


    Clip*                       getClip() { return m_clip; }

    //void                      setInputMedia( InputMedia* inputMedia ) { m_inputMedia = inputMedia; }

private:
    /**
     *  \brief fileType member
     */
    ListViewMediaItem::fType    m_fileType;

    /**
     * \Instance of the InputMedia
     */
    Clip*                       m_clip;

    /**
     * \brief Instance of the temporary QWidget use for the snapshot
     */
    QWidget*                    m_renderWidget;

private slots:
    void                    snapshotChanged();
};

#endif /* !LISTVIEWMEDIAITEM_H */
