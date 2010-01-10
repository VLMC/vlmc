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
#include "GraphicsTrack.hpp"

#define RESIZE_ZONE 7

class TracksView;
class Clip;

/**
 * \brief Base class for Audio/Video items.
 */
class AbstractGraphicsMediaItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    friend class TracksView;
public:
    enum From
    {
        BEGINNING,
        END
    };

    AbstractGraphicsMediaItem();
    virtual ~AbstractGraphicsMediaItem() { }

    /// Defines the outer bounds of the item as a rectangle
    virtual QRectF boundingRect() const;

    /// Return the Type of the MediaItem (see http://doc.trolltech.com/4.5/qgraphicsitem.html#type)
    virtual int type() const = 0;

    /// The item length can be expanded or shrinked by the user.
    virtual bool expandable() const = 0;

    /// The item can be moved by the user.
    virtual bool moveable() const = 0;

    /// Should return the unique uid of the contained media.
    virtual const QUuid& uuid() const = 0;

    /// Clip contained in the item
    virtual Clip* clip() const = 0;

    /// Return the type of the media
    virtual MainWorkflow::TrackType mediaType() const = 0;

    /// Group two items together
    void group( AbstractGraphicsMediaItem* item );

    /// Ungroup two items
    void ungroup();

    /// Return the current track of the item
    quint32 trackNumber();

    /// Set the position of the item (in frames) for the x-axis.
    void setStartPos( qint64 position );

    /// Return the position of the item (in frames) for the x-axis.
    qint64 startPos();

    /// Resize an item from its beginning or from its end.
    void resize( qint64 size, From from = BEGINNING );

protected:
    /**
     * \details Returns a pointer to the tracksView which contains the item,
     * or NULL if the item is not stored in a tracksView.
     */
    TracksView* tracksView();

    /**
     * \brief Contains the old trackNumber.
     */
    unsigned int oldTrackNumber;
    /**
     * \brief Contains the old position.
     */
    qint64 oldPosition;

    /**
     * \brief Return a pointer to the linked item.
     * \details This method will return NULL if there is no linked item.
     */
    AbstractGraphicsMediaItem* groupItem();

    /**
     * \brief Set the width of the item.
     * \param width Width in frames.
     */
    void setWidth( qint64 width );
    /**
     * \brief Set the height of the item.
     * \param height Height in pixels.
     */
    void setHeight( qint64 height );

protected slots:
    /**
     * \brief Adjust the length of the item according to the associated Clip.
     * \details This method should be called when the clip size change
     */
    void adjustLength();

private:
    /// This pointer will be set when inserted in the tracksView.
    TracksView* m_tracksView;

    /// Pointer used to save the address of a linked item.
    AbstractGraphicsMediaItem* m_group;

    qint64  m_width;
    qint64  m_height;
    //FIXME: this is a nasty forest boolean
    bool    m_resizeExpected;
};

#endif // ABSTRACTGRAPHICSMEDIAITEM_H
