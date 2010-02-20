/*****************************************************************************
 * SettingValue.cpp: A setting value that can broadcast its changes
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
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
#include <QtDebug>

SettingValue::SettingValue( const QVariant& defaultValue, const QString& desc ) :
        m_val( defaultValue ),
        m_defaultVal( defaultValue ),
        m_desc( desc )
{
}

void
SettingValue::set( const QVariant& val )
{
    if ( val != m_val )
    {
        m_val = val;
        emit changed( m_val );
    }
}

const QVariant&
SettingValue::get() const
{
    return m_val;
}

const QString&
SettingValue::description() const
{
    return m_desc;
}

void
SettingValue::restoreDefault()
{
    set( m_defaultVal );
}
