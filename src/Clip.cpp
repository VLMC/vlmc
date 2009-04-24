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

#include "Clip.h"

Clip::Clip( Media* parent, qint64 begin, qint64 end ) : m_parent( parent ), m_begin( begin ), m_end( end )
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

Media*      Clip::getParent() const
{
    return m_parent;
}

qint64      Clip::getLength() const
{
    if ( m_begin == Clip::UntilEndOfMedia )
        return m_parent->getLength() - m_begin;
    return m_end - m_begin;
}
