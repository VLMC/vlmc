/*****************************************************************************
 * AbstractGraphicsMediaItem.h: Base class for media representation
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

#ifndef ABSTRACTGRAPHICSMEDIAITEM_H
#define ABSTRACTGRAPHICSMEDIAITEM_H

#include <QGraphicsItem>
#include <QUuid>

class AbstractGraphicsMediaItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    virtual ~AbstractGraphicsMediaItem() { }

    /// The item length can be expanded or shrinked by the user.
    virtual bool expandable() const = 0;

    /// The item can be moved by the user.
    virtual bool moveable() const = 0;

    /// Should return the unique uid of the contained media.
    virtual const QUuid& uuid() const = 0;
};

#endif // ABSTRACTGRAPHICSMEDIAITEM_H
