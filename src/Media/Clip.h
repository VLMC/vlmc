/*****************************************************************************
 * Clip.h : Represents a basic container for media informations.
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
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
  * This file contains the Clip class declaration/definition.
  * It's used by the timeline in order to represent a subset of a media
  */

#ifndef CLIP_H__
# define CLIP_H__

#include <QObject>
#include <QUuid>

#include "Media.h"


class Media;

class   Clip : public QObject
{
    Q_OBJECT

    public:
        static const int DefaultFPS;
        Clip( Media* parent );
        Clip( Media* parent, qint64 begin, qint64 end = -1 );
        Clip( Clip* creator, qint64 begin, qint64 end );
        Clip( Clip* clip );
        Clip( const QUuid& uuid, qint64 begin = 0, qint64 end = -1 );
        virtual ~Clip();

        qint64              getBegin() const;
        qint64              getEnd() const;

        void                setBegin( qint64 begin, bool updateMax = false );
        void                setEnd( qint64 end, bool updateMax = false );
        void                setBoundaries( qint64 newBegin, qint64 newEnd, bool updateMax = false );

        /**
            \return         Returns the clip length in frame.
        */
        qint64              getLength() const;

        /**
            \return         Returns the clip length in seconds.
        */
        qint64              getLengthSecond() const;

        /**
            \return         Returns the Media that the clip was basep uppon.

        */
        Media*              getParent();

        /**
            \brief          Returns an unique Uuid for this clip (which is NOT the
                            parent's Uuid).

            \return         The Clip's Uuid as a QUuid
        */
        const QUuid&        getUuid() const;

        const QStringList&  getMetaTags() const;
        void                setMetaTags( const QStringList& tags );
        bool                matchMetaTag( const QString& tag ) const;

        const QString&      getNotes() const;
        void                setNotes( const QString& notes );

        qint64              getMaxBegin() const;
        qint64              getMaxEnd() const;

    private:
        void        computeLength();

        Media*      m_parent;
        /**
         *  \brief  This represents the beginning of the Clip in frames, from the
         *          beginning of the parent Media.
         */
        qint64      m_begin;
        /**
         *  \brief  This represents the end of the Clip in frames, from the
         *          beginning of the parent Media.
         */
        qint64      m_end;

        /**
         *  \brief  The length in frames
         */
        qint64      m_length;
        /**
         *  \brief  The length in seconds (Be carreful, VLC uses MILLIseconds)
         */
        qint64      m_lengthSeconds;
        /**
         *  The Clip's timeline UUID. Used to identify the Clip in the
         *  timeline, as a unique object, even if this clip is present more than
         *  once.
         */
        QUuid       m_Uuid;
        QStringList m_metaTags;
        QString     m_notes;

        /**
         *  This is used for the resize. The clip won't be abble to be resized beyond this value.
         *  ie this clip won't start before m_maxBegin.
         */
        qint64      m_maxBegin;

        /**
         *  This is used for the resize. The clip won't be abble to be resized beyond this value
         *  ie this clip won't end before m_maxEnd.
         */
        qint64      m_maxEnd;

    signals:
        void        lengthUpdated();
};

#endif //CLIP_H__
