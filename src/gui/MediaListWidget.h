/*****************************************************************************
 * MediaListWidget.h: Multimedia list widget
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

#ifndef MEDIALISTWIDGET_H
#define MEDIALISTWIDGET_H

#include <QListWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QMimeData>
#include <QDrag>
#include <QApplication>
#include <QSvgRenderer>
#include "ListViewMediaItem.h"

class MediaListWidget : public QListWidget
{
    Q_OBJECT
public:
    MediaListWidget( QWidget* parent = 0 );
    void setType( ListViewMediaItem::fType fileType );
protected:
    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void paintEvent( QPaintEvent* event );
private:
    ListViewMediaItem::fType m_Type;
    QPoint m_dragStartPos;
    QSvgRenderer* m_svgRenderer;
};

#endif // MEDIALISTWIDGET_H
