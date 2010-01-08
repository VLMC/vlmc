/*****************************************************************************
 * SettingValue.cpp: A setting value that can broadcast its changes
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

#include "SettingValue.h"

SettingValue::SettingValue( const QVariant& val ) : m_val ( val )
{
}

void        SettingValue::set( const QVariant& val )
{
    if ( val != m_val )
    {
        m_val = val;
        emit changed( m_val );
    }
}

const QVariant&     SettingValue::get() const
{
    return m_val;
}
