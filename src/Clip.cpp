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

#include "Clip.h"

Clip::Clip( Media* parent ) : m_parent( parent ), m_begin( 0.0f ), m_end( 1.0f )
{
    init();
}

Clip::Clip( Clip* creator, float begin, float end ) : m_parent( creator->getParent() ), m_begin( begin ), m_end( end )
{
    init();
}

Clip::Clip( Media* parent, float begin, float end ) : m_parent( parent ), m_begin( begin ), m_end( end )
{
    init();
}

Clip::~Clip()
{
}

void        Clip::init()
{
    m_uuid = QUuid::createUuid();
    computeLength();
}

float       Clip::getBegin() const
{
    return m_begin;
}

float       Clip::getEnd() const
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

void        Clip::computeLength()
{
    float   fps = m_parent->getFps();
    if ( fps < 0.1f )
        fps = FPS;
    qint64 nbMs = (qint64)( ( m_end - m_begin ) * (float)m_parent->getLength() );
    m_length = (nbMs / 1000) * fps;
}

const QUuid&    Clip::getUuid() const
{
    return m_uuid;
}
