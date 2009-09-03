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

//TODO: REMOVE THIS
#ifndef FPS
#define FPS     30
#endif

class   Clip : public QObject
{
    Q_OBJECT

    public:
        Clip( Media* parent );
        Clip( Media* parent, float begin, float end );
        Clip( Clip* creator, float begin, float end );
        Clip( Clip* clip );
        virtual ~Clip();

        /**
          \brief    Returns the clip starting point. This value will be in
                    vlc positition units (0 to 1)
          \return   A value between 0 and 1, where 0 is the real Media begin,
                    and 1 the real Media end.
        */
        float           getBegin() const;

        /**
          \brief    Returns the clip ending point. This value will be in
                    vlc positition units (0 to 1)
          \return   A value between 0 and 1, where 0 is the real Media end,
                    and 1 the real Media end.
        */
        float           getEnd() const;

        void            setBegin( float begin );
        void            setEnd( float end );

        /**
            \return     Returns the clip length in frame.
        */
        qint64          getLength() const;

        /**
            \return     Returns the clip length in seconds.
        */
        qint64          getLengthSecond() const;

        /**
            \return     Returns the Media that the clip was basep uppon.

        */
        Media*          getParent();

        /**
            \brief      Returns an unique Uuid for this clip (which is NOT the
                        parent's Uuid).

            \return     The Clip's Uuid as a QUuid
        */
        const QUuid&        getUuid() const;

        const QStringList&  getMetaTags() const;
        void                setMetaTags( const QStringList& tags );
        bool                matchMetaTag( const QString& tag ) const;

        const QString&      getNotes() const;
        void                setNotes( const QString& notes );

    private:
        void        computeLength();

        Media*      m_parent;
        float       m_begin;
        float       m_end;
        qint64      m_length;
        qint64      m_lengthSeconds;
        /**
         *  The Clip's timeline UUID. Used to identify the Clip in the
         *  timeline, as a unique object, even if this clip is present more than
         *  once.
         */
        QUuid       m_Uuid;
        QStringList m_metaTags;
        QString     m_notes;

};

#endif //CLIP_H__
