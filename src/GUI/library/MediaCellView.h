/*****************************************************************************
 * MediaCellView.h
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Thomas Boquet <thomas.boquet@gmail.com>
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

#ifndef MEDIACELLVIEW_H
#define MEDIACELLVIEW_H

#include <QWidget>
#include <QUuid>
#include <QMouseEvent>
#include "ClickableLabel.h"

namespace Ui
{
    class MediaCellView;
}

class MediaCellView : public QWidget
{
    Q_OBJECT

public:
    MediaCellView( const QUuid& uuid, QWidget *parent = 0 );
    ~MediaCellView();

    void                    setTitle( const QString& title );
    void                    setThumbnail( const QPixmap& pixmap );
    const QPixmap*          getThumbnail() const;
    /**
     *  \brief  Set the length displayed in the cell
     *  \param  length  The media length, in ms.
     */
    void                    setLength( qint64 length, bool mSecs = true );
    void                    incrementClipCount();
    void                    decrementClipCount( const int nb );
    QString                 title() const;
    const QUuid&            uuid() const;
    const ClickableLabel*   nextButton() const;
    const ClickableLabel*   deleteButton() const;

protected:
    void changeEvent( QEvent *e );

private:
    Ui::MediaCellView*  m_ui;
    const QUuid         m_uuid;
    QPoint              m_dragStartPos;

protected:
    void        mouseDoubleClickEvent( QMouseEvent* );
    void        mousePressEvent( QMouseEvent* );
    void        mouseMoveEvent( QMouseEvent* );

signals:
    void        cellSelected( const QUuid& uuid );
    void        arrowClicked( const QUuid& uuid );
    void        cellDeleted( const QUuid& uuid );

public slots:
    void        deleteButtonClicked( QWidget* sender, QMouseEvent* event );
    void        arrowButtonClicked( QWidget* sender, QMouseEvent* event );

};

#endif // MEDIACELLVIEW_H
