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

#include "SettingsManager.h"

#include "VLMCSettingsDefault.h"
#include "ProjectSettingsDefault.h"

#include <QHash>
#include <QDomElement>
#include <QDomNamedNodeMap>
#include <QDomNodeList>
#include <QtDebug>
#include <QWriteLocker>
#include <QReadLocker>
#include <QTextStream>



bool    SettingsManager::m_defaultLoaded = false;

SettingsManager::SettingsManager( QObject* parent )
    : QObject( parent )
{
}

SettingsManager::~SettingsManager()
{
}

void  SettingsManager::setValue( const QString& part , const QString& key, const QVariant& value )
{
    m_globalLock.lockForRead();
    if ( !m_tempData.contains( part ) )
    {
        m_globalLock.unlock();
        addNewSettingsPart( part );
    }
    else
        m_globalLock.unlock();
    QWriteLocker    lock( &m_globalLock );
    SettingsPart*   tmp = m_tempData[part];
    SettingsPart::ConfigPair::iterator it = tmp->m_data.find( key );
    if ( it == tmp->m_data.end() )
        tmp->m_data[key] = new SettingValue( value );
    else
        it.value()->set( value );
    return ;
}

const SettingValue*     SettingsManager::getValue( const QString& part, const QString& key ) const
{
    if ( !m_data.contains( part ) )
        return getValue( "default", key );
    QReadLocker readLock( &m_globalLock );
    QReadLocker rdLock( &m_data[part]->m_lock );
    if ( m_data[part]->m_data.contains( key ) == true )
        return m_data[part]->m_data[key];
    return NULL;
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
    SettingsPart::ConfigPair::const_iterator  it = sett->m_data.begin();
    SettingsPart::ConfigPair::const_iterator  end = sett->m_data.end();
    QDomElement settingsNode = xmlfile.createElement( part );
    for ( ; it != end; ++it )
    {
        QDomElement elem = xmlfile.createElement( it.key() );
        elem.setAttribute( "value", it.value()->get().toString() );
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
                new SettingValue( QVariant( attrMap.item( 0 ).nodeValue() ) ) );
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
    {
        QWriteLocker     lock( &m_globalLock );

        QHash<QString, SettingsPart*>::iterator it = m_tempData.begin();
        QHash<QString, SettingsPart*>::iterator ed = m_tempData.end();

        for ( ; it != ed; ++it )
        {
            SettingsPart*   sett = it.value();
            QString         part = it.key();

            QReadLocker rLock( &sett->m_lock );
            SettingsPart::ConfigPair::iterator iter = sett->m_data.begin();
            SettingsPart::ConfigPair::iterator end = sett->m_data.end();
            QWriteLocker    wLock( &m_data[part]->m_lock );
            for ( ; iter != end; ++iter )
            {
                QString                                 settingName = iter.key();
                SettingsPart::ConfigPair::iterator      insert_it = m_data[part]->m_data.find( settingName );

                if ( insert_it == m_data[part]->m_data.end() )
                    m_data[part]->m_data.insert( settingName, new SettingValue( iter.value()->get() ) );
                else
                    m_data[part]->m_data[ settingName ]->set( iter.value()->get() );
            }
        }
    }
    flush();
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
    if ( !SettingsManager::m_defaultLoaded )
    {
        SettingsManager::m_defaultLoaded = true;
        VLMCSettingsDefault::load( "default" );
        VLMCSettingsDefault::load( "VLMC" );
        ProjectSettingsDefault::load( "default" );
        ProjectSettingsDefault::load( "project" );
    }
}

SettingsManager*    SettingsManager::getInstance()
{
    SettingsManager*    ret = QSingleton<SettingsManager>::getInstance();
    if ( !SettingsManager::m_defaultLoaded )
    {
        SettingsManager::loadDefaultsSettings();
    }
    return ret;
}

const SettingsPart*     SettingsManager::getConfigPart( const QString& part ) const
{
    QHash<QString, SettingsPart*>::const_iterator      it = m_data.find( part );

    if ( it == m_data.end() )
        return NULL;
    return it.value();
}
