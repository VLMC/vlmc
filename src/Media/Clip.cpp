/*****************************************************************************
 * Clip.cpp : Represents a basic container for media informations.
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
  * This file contains the Clip class implementation.
  */

#include <QtDebug>
#include "Library.h"

#include "Clip.h"

Clip::Clip( Media* parent ) : m_parent( parent ), m_begin( 0 ), m_end( parent->getNbFrames() )
{
    m_Uuid = QUuid::createUuid();
    computeLength();
}

Clip::Clip( Clip* creator, qint64 begin, qint64 end ) : m_parent( creator->getParent() ), m_begin( begin ), m_end( end )
{
    m_Uuid = QUuid::createUuid();
    computeLength();
}

Clip::Clip( Media* parent, qint64 begin, qint64 end ) : m_parent( parent ), m_begin( begin ), m_end( end )
{
    Q_ASSERT( parent->getInputType() == Media::File || ( begin == 0 && end == m_parent->getNbFrames() ) );
    m_Uuid = QUuid::createUuid();
    computeLength();
}

Clip::Clip( Clip* clip ) :
        m_parent( clip->m_parent ),
        m_begin( clip->m_begin ),
        m_end( clip->m_end ),
        m_length( clip->m_length ),
        m_lengthSeconds( clip->m_lengthSeconds ),
        m_metaTags( clip->m_metaTags ),
        m_notes( clip->m_notes )
{
    m_Uuid = QUuid::createUuid();
}

Clip::Clip( const QUuid& uuid, qint64 begin, qint64 end ) :
        m_begin( begin),
        m_end( end )
{
    Q_UNUSED( end );
    Media*  media = Library::getInstance()->getMedia( uuid );
    Q_ASSERT( media != NULL );
    m_parent = media;
    m_Uuid = QUuid::createUuid();
    computeLength();
}

Clip::~Clip()
{
}

qint64      Clip::getBegin() const
{
    return m_begin;
}

qint64      Clip::getEnd() const
{
    return m_end;
}

Media*      Clip::getParent()
{
    return m_parent;
}

qint64      Clip::getLength() const
{
    return m_length;
}

qint64      Clip::getLengthSecond() const
{
    return m_lengthSeconds;
}

void        Clip::computeLength()
{
    if ( m_parent->getInputType() == Media::File )
    {
        unsigned int   fps = m_parent->getFps();
        if ( fps < 0.1f )
            fps = FPS;
        m_length = m_end - m_begin;
        m_lengthSeconds = m_length * fps;
        emit lengthUpdated();
    }
    else
    {
        m_length = 0;
        m_lengthSeconds = 0;
    }
}

const QStringList&      Clip::getMetaTags() const
{
    return m_metaTags;
}

void            Clip::setMetaTags( const QStringList& tags )
{
    m_metaTags = tags;
}

bool            Clip::matchMetaTag( const QString& tag ) const
{
    if ( tag.length() == 0 )
        return true;
    if ( m_parent->matchMetaTag( tag ) == true )
        return true;
    QString metaTag;
    foreach ( metaTag, m_metaTags )
    {
        if ( metaTag.startsWith( tag, Qt::CaseInsensitive ) == true )
            return true;
    }
    return false;
}

const   QString&    Clip::getNotes() const
{
    return m_notes;
}

void                Clip::setNotes( const QString& notes )
{
    m_notes = notes;
}

const QUuid&        Clip::getUuid() const
{
    Q_ASSERT( m_Uuid.isNull() == false );
    return m_Uuid;
}

void                Clip::setBegin( qint64 begin )
{
    Q_ASSERT( begin >= .0f );
    if ( begin == m_begin ) return;
    m_begin = begin;
    computeLength();
    emit lengthUpdated();
}

void                Clip::setEnd( qint64 end )
{
    if ( end == m_end ) return;
    m_end = end;
    computeLength();
    emit lengthUpdated();
}

Clip*               Clip::split( qint64 endFrame )
{
    Q_ASSERT( endFrame != m_end );
    endFrame += m_begin;
    Clip*   newClip = new Clip( this, endFrame, m_end );
    m_end = endFrame;
    computeLength();
    return newClip;
}
