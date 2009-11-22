/*****************************************************************************
 * GraphicsCursorItem.h: Timeline's cursor
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Ludovic Fauvet <etix@l0cal.com>
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

#ifndef GRAPHICSCURSORITEM_H
#define GRAPHICSCURSORITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QPen>
#include <QRectF>
#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include "GenericRenderer.h"

class GraphicsCursorItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    GraphicsCursorItem( const QPen& pen );
    int cursorPos() const { return ( int )pos().x(); }
    virtual QRectF boundingRect() const;
    void setHeight( int height );

protected:
    virtual void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0 );
    virtual QVariant itemChange( GraphicsItemChange change, const QVariant& value );

private:
    QPen        m_pen;
    QRectF      m_boundingRect;

signals:
    void cursorPositionChanged( qint64 pos );

public slots:
    void frameChanged( qint64 position, MainWorkflow::FrameChangedReason );
};

#endif // GRAPHICSCURSORITEM_H
