/*****************************************************************************
 * SettingsManager.cpp * : Backend settings manager
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Clement CHAVANCE <kinder@vlmc.org>
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

#include <QHash>
#include <QDomElement>

#include "SettingsManager.h"

SettingsManager::SettingsManager( QObject* parent )
    : QObject( parent )
{
}

SettingsManager::~SettingsManager()
{
}

void  SettingsManager::setValues( QHash<QString, QVariant> values )
{
    QHash<QString, QVariant>::iterator  it = values.begin();
    QHash<QString, QVariant>::iterator  end = values.end();

    for ( ; it != end; ++it  )
        m_data.insert( it.key(), it.value() );
    return ;
}

void  SettingsManager::setValue( const QString& key, QVariant& value )
{
    m_data.insert( key, value );
    return ;
}

const QVariant   SettingsManager::getValue( const QString& key ) const
{
    return m_data[key];
}

void  SettingsManager::saveSettings( QDomDocument& xmlfile, QDomElement& root )
{
    m_lock.lockForRead();

    //SAVE SETTINGS TO DomDocument
    QHash<QString, QVariant>::iterator  it = m_data.begin();
    QHash<QString, QVariant>::iterator  end = m_data.end();
    QDomElement settingsNode = xmlfile.createElement( "settings" );
    for ( ; it != end; ++it )
        settingsNode.setAttribute( it.key(), it.value().toString() );
    m_lock.unlock();

    root.appendChild( settingsNode );
}
