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

bool    SettingsManager::m_defaultLoaded = false;

SettingsManager::SettingsManager( QObject* parent )
    : QObject( parent )
{
}

SettingsManager::~SettingsManager()
{
}

void  SettingsManager::setValues( const QString& part, QHash<QString, QVariant> values )
{
    if ( !m_tempData.contains( part ) )
        addNewSettingsPart( part );
    m_globalLock.lockForRead();
    SettingsPart* sett = m_tempData[part];
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
    if ( !m_tempData.contains( part ) )
    {
        addNewSettingsPart( part );
    }
    m_globalLock.unlock();
    QWriteLocker    lock( &m_globalLock );
    SettingsPart*   tmp = m_tempData[part];
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

    root.appendChild( settingsNode );
}

void  SettingsManager::loadSettings( const QString& part, const QDomElement& settings )
{
    if ( settings.isNull() == true || settings.tagName() != "project" )
    {
        qWarning() << "Invalid settings node";
        return ;
    }
    m_globalLock.lockForRead();
    if ( !m_data.contains( part ) )
    {
        qWarning() << "These settings Does not exists";
        return ;
    }
    SettingsPart*   sett = m_data[part];
    m_globalLock.unlock();
    //Loading all the settings
    m_globalLock.lockForWrite();
    sett->m_lock.lockForWrite();
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
        sett->m_data.insert( list.at( idx ).toElement().tagName(),
                QVariant( attrMap.item( 0 ).nodeValue() ));
    }
    sett->m_lock.unlock();
    m_globalLock.unlock();
    emit settingsLoaded();
}

void  SettingsManager::addNewSettingsPart( const QString& name )
{
    QReadLocker rLock( &m_globalLock );
    if ( !m_data.contains( name ) )
    {
        rLock.unlock();
        QWriteLocker    lock( &m_globalLock );
        m_data.insert( name, new SettingsPart );
        m_tempData.insert( name, new SettingsPart );
    }
}

void    SettingsManager::commit()
{
    QWriteLocker     lock( &m_globalLock );

    QHash<QString, SettingsPart*>::iterator it = m_tempData.begin();
    QHash<QString, SettingsPart*>::iterator ed = m_tempData.end();

    for ( ; it != ed; ++it )
    {
        SettingsPart*   sett = it.value();

        QReadLocker rLock( &sett->m_lock );
        QHash<QString, QVariant>::iterator iter = sett->m_data.begin();
        QHash<QString, QVariant>::iterator ed = sett->m_data.end();
        QWriteLocker    wLock( &m_data[it.key()]->m_lock );
        for ( ; iter != ed; ++iter )
            m_data[it.key()]->m_data.insert( iter.key(), iter.value() );
    }
}

void    SettingsManager::flush()
{
    QWriteLocker     lock( &m_globalLock );

    QHash<QString, SettingsPart*>::iterator it = m_tempData.begin();
    QHash<QString, SettingsPart*>::iterator ed = m_tempData.end();

    for ( ; it != ed; ++it )
    {
        QWriteLocker    wLock( &it.value()->m_lock );
        it.value()->m_data.clear();
    }
}

void  SettingsManager::loadDefaultsSettings()
{
    VLMCSettingsDefault::load( "default" );
    VLMCSettingsDefault::load( "VLMC" );
    ProjectSettingsDefault::load( "default" );
    ProjectSettingsDefault::load( "project" );
}

SettingsManager*    SettingsManager::getInstance()
{
    SettingsManager*    ret = QSingleton<SettingsManager>::getInstance();
    if ( !SettingsManager::m_defaultLoaded )
    {
        SettingsManager::m_defaultLoaded = true;
        SettingsManager::loadDefaultsSettings();
    }
    return ret;
}

