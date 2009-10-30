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
#include <QWriteLocker>
#include <QReadLocker>
#include <QTextStream>

#include "SettingsManager.h"
#include "VLMCSettingsDefault.h"
#include "ProjectSettingsDefault.h"


SettingsManager::SettingsManager( QObject* parent )
    : QObject( parent )
{
}

SettingsManager::~SettingsManager()
{
}

void  SettingsManager::setValues( const QString& part, QHash<QString, QVariant> values )
{
    if ( !m_data.contains( part ) )
        addNewSettingsPart( part );
    m_globalLock.lockForRead();
    SettingsPart* sett = m_data[part];
    m_globalLock.unlock();
    QHash<QString, QVariant>::iterator  it = values.begin();
    QHash<QString, QVariant>::iterator  end = values.end();

    QWriteLocker    lock( &sett->m_lock );
    for ( ; it != end; ++it  )
        sett->m_data.insert( it.key(), it.value() );
    return ;
}

void  SettingsManager::setValue( const QString& part , const QString& key, QVariant& value )
{
    m_globalLock.lockForRead();
    if ( !m_data.contains( part ) )
        addNewSettingsPart( part );
    m_globalLock.unlock();
    QWriteLocker    lock( &m_globalLock );
    SettingsPart*   tmp = m_data[part];
    tmp->m_data.insert( key, value );
    return ;
}

const QVariant&   SettingsManager::getValue( const QString& part, const QString& key ) const
{
    if ( !m_data.contains( part ) )
        return getValue( "default", key );
    QReadLocker readLock( &m_globalLock );
    QReadLocker rdLock( &m_data[part]->m_lock );
    QVariant&  value = m_data[part]->m_data[key];
    return value;
}

void  SettingsManager::saveSettings( const QString& part, QDomDocument& xmlfile, QDomElement& root )
{
    m_globalLock.lockForRead();
    if ( !m_data.contains( part ) )
    {
        m_globalLock.unlock();
        return ;
    }
    SettingsPart*   sett = m_data[part];
    m_globalLock.unlock();
    //SAVE SETTINGS TO DomDocument
    QReadLocker     lock( &sett->m_lock );
    QHash<QString, QVariant>::iterator  it = sett->m_data.begin();
    QHash<QString, QVariant>::iterator  end = sett->m_data.end();
    QDomElement settingsNode = xmlfile.createElement( part );
    for ( ; it != end; ++it )
    {
        QDomElement elem = xmlfile.createElement( it.key() );
        elem.setAttribute( "value", it.value().toString() );
        settingsNode.appendChild( elem );
    }

    //DEBUG
    {
        QTextStream stream( stdout );
        stream << settingsNode;
    }

    root.appendChild( settingsNode );
}

void  SettingsManager::loadSettings( const QDomElement& settings )
{
    //qDebug() << "Loading settings";
    //if ( settings.isNull() == true || settings.tagName() != "settings" )
    //{
    //    qWarning() << "Invalid settings node";
    //    return ;
    //}
    ////Loading all the settings
    //m_globalLock.lockForWrite();

    //QDomNodeList  list = settings.childNodes();
    //int           nbChild = list.size();

    //for ( int idx = 0; idx < nbChild; ++idx )
    //{
    //    QDomNamedNodeMap  attrMap = list.at( idx ).attributes();
    //    if ( attrMap.count() > 1 )
    //    {
    //        qWarning() << "Invalid number of attributes for" << list.at( idx ).nodeName();
    //        return ;
    //    }
    //    m_data.insert( list.at( idx ).toElement().tagName(),
    //            QVariant( attrMap.item( 0 ).nodeValue() ));
    //}
    //m_lock.unlock();
    //emit settingsLoaded();
}

void  SettingsManager::addNewSettingsPart( const QString& name )
{
    QWriteLocker    lock( &m_globalLock );
    m_data.insert( name, new SettingsPart );
}

void  SettingsManager::loadDefaultsSettings()
{
    VLMCSettingsDefault::load();
    ProjectSettingsDefault::load();
}

