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
#include <QDomNamedNodeMap>
#include <QDomNodeList>
#include <QtDebug>
#include <QTextStream>

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

    m_lock.lockForWrite();
    for ( ; it != end; ++it  )
        m_data.insert( it.key(), it.value() );
    m_lock.unlock();
    return ;
}

void  SettingsManager::setValue( const QString& key, QVariant& value )
{
    m_lock.lockForWrite();
    m_data.insert( key, value );
    m_lock.unlock();
    return ;
}

const QVariant   SettingsManager::getValue( const QString& key ) const
{
    m_lock.lockForRead();
    QVariant  value = m_data[key];
    m_lock.unlock();
    return value;
}

void  SettingsManager::saveSettings( QDomDocument& xmlfile, QDomElement& root )
{
    m_lock.lockForRead();

    //SAVE SETTINGS TO DomDocument
    QHash<QString, QVariant>::iterator  it = m_data.begin();
    QHash<QString, QVariant>::iterator  end = m_data.end();
    QDomElement settingsNode = xmlfile.createElement( "settings" );
    for ( ; it != end; ++it )
    {
        QDomElement elem = xmlfile.createElement( it.key() );
        elem.setAttribute( "value", it.value().toString() );
        settingsNode.appendChild( elem );
    }
    m_lock.unlock();

    //DEBUG
    {
        QTextStream stream( stdout );
        stream << settingsNode;
    }

    root.appendChild( settingsNode );
}

void  SettingsManager::loadSettings( const QDomElement& settings )
{
    qDebug() << "Loading settings";
    if ( settings.isNull() == true || settings.tagName() != "settings" )
    {
        qWarning() << "Invalid settings node";
        return ;
    }
    //Loading all the settings
    m_lock.lockForWrite();

    QDomNodeList  list = settings.childNodes();
    int           nbChild = list.size();

    for ( int idx = 0; idx < nbChild; ++idx )
    {
        QDomNamedNodeMap  attrMap = list.at( idx ).attributes();
        if ( attrMap.count() > 1 )
        {
            qWarning() << "Invalid number of attributes for" << list.at( idx ).nodeName();
            return ;
        }
        m_data.insert( list.at( idx ).toElement().tagName(),
                QVariant( attrMap.item( 0 ).nodeValue() ));
    }
    m_lock.unlock();
    emit settingsLoaded( m_data );
}

