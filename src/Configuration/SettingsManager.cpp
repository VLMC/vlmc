/*****************************************************************************
 * SettingsManager.cpp: Backend settings manager
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

#include "SettingValue.h"

#include <QSettings>
#include <QDomDocument>
#include <QWriteLocker>
#include <QReadLocker>
#include <QXmlStreamWriter>

#include <QtDebug>

void
SettingsManager::setValue( const QString &key,
                           const QVariant &value,
                           SettingsManager::Type type )
{
    if ( type == XML )
        m_tmpXmlSettings.insert( key, new SettingValue( value ) );
    else if ( type == QSett )
        m_tmpClassicSettings.insert( key, new SettingValue( value ) );
    return ;
}

void
SettingsManager::setImmediateValue( const QString &key,
                                    const QVariant &value,
                                    SettingsManager::Type type )
{
    QWriteLocker    wlock( &m_rwLock );
    SettingHash  *settMap;
    if ( type == XML )
        settMap = &m_xmlSettings;
    else if ( type == QSett )
    {
        QSettings    sett;
        sett.setValue( key, value );
        sett.sync();
        settMap = &m_classicSettings;
    }
    if ( settMap->contains( key ) )
        settMap->value( key )->set( value );
    else
        settMap->insert( key, new SettingValue( value ) );

    return ;
}

QVariant
SettingsManager::value( const QString &key,
                        const QVariant &defaultValue,
                        SettingsManager::Type type )
{
    qDebug() << "value :" << key << "default" << defaultValue;
    QReadLocker rl( &m_rwLock );

    if ( ( type == XML || type == All ) && m_xmlSettings.contains( key ) )
        return m_xmlSettings.value( key )->get();
    else if ( ( type == QSett || type == All ) )
    {
        if ( m_classicSettings.contains( key ) )
            return m_classicSettings.value( key )->get();
        else
        {
            QSettings    sett;
            QVariant val = sett.value( key, defaultValue );
            if ( val != defaultValue )
                m_classicSettings.insert( key, new SettingValue( val ) );

            return val;
        }
    }

    return defaultValue;
}

QHash<QString, QVariant>
SettingsManager::group( const QString& groupName, SettingsManager::Type type )
{
}

bool
SettingsManager::watchValue( const QString &key,
                             QObject* receiver,
                             const char *method,
                             SettingsManager::Type type )
{
    QReadLocker rl( &m_rwLock );

    if ( ( type == XML || type == All ) && m_xmlSettings.contains( key ) )
    {
        connect( m_xmlSettings[key], SIGNAL( changed( const QVariant& ) ),
                 receiver, method );
        return true;
    }
    else if ( ( type == QSett || type == All ) )
    {
        if ( m_classicSettings.contains( key ) )
        {
            connect( m_classicSettings[key], SIGNAL( changed( const QVariant& ) ),
                    receiver, method );
            return true;
        }
    }

    return false;
}

void
SettingsManager::save() const
{
    QReadLocker rl( &m_rwLock );
    QSettings    sett;
    SettingHash::const_iterator it = m_classicSettings.begin();
    SettingHash::const_iterator ed = m_classicSettings.end();

    for ( ; it != ed; ++it )
        sett.setValue( it.key(), it.value()->get() );
    sett.sync();
}

void
SettingsManager::save( QXmlStreamWriter &stream ) const
{
    typedef QPair<QString, SettingValue*> settingPair;
    QMultiHash<QString, settingPair>  parts;

    QReadLocker rl( &m_rwLock );
    SettingHash::const_iterator it;
    SettingHash::const_iterator ed = m_xmlSettings.end();

    for ( it = m_xmlSettings.begin(); it != ed; ++it )
    {
        QString key = it.key();
        if ( key.count( "/" ) == 1 )
        {
            int idx = key.indexOf( "/" );
            QString part = key.left( idx );
            QString name = key.right( idx );

            parts.insert( part, settingPair( name, it.value() ) );
        }
    }
    QMultiHash<QString, settingPair>::const_iterator iter;
    QMultiHash<QString, settingPair>::const_iterator end = parts.end();
    stream.writeStartElement( "Settings" );
    for ( iter = parts.begin(); iter != end; ++iter )
    {
        stream.writeStartElement( iter.key() );
        QList<settingPair>  pairs = parts.values( iter.key() );
        foreach( settingPair pair, pairs )
        {
            stream.writeStartElement( pair.first );
            stream.writeAttribute( "value", pair.second->get().toString() );
            stream.writeEndElement();
        }
        stream.writeEndElement();
    }
    stream.writeEndElement();
}

bool
SettingsManager::load( QDomElement &element )
{
}

bool
SettingsManager::commit( SettingsManager::Type type )
{
    {
        QWriteLocker    wlock( &m_rwLock );
        if ( type == XML || type == All )
        {
            SettingHash::iterator it;
            SettingHash::iterator ed = m_tmpXmlSettings.end();
            for ( it = m_tmpXmlSettings.begin() ; it != ed; ++it )
            {
                if ( m_xmlSettings.contains( it.key() ) )
                    m_xmlSettings[it.key()]->set( it.value()->get() );
                else
                    m_xmlSettings.insert( it.key(), it.value() );
            }
        }
        if ( type == QSett || type == All )
        {
            QSettings sett;
            SettingHash::iterator it;
            SettingHash::iterator ed = m_classicSettings.end();
            for ( it = m_classicSettings.begin(); it != ed; ++it )
            {
                sett.setValue( it.key(), it.value()->get() );
                if ( m_classicSettings.contains( it.key() ) )
                    m_classicSettings[it.key()]->set( it.value()->get() );
                else
                    m_classicSettings.insert( it.key(), it.value() );
            }
        }
    }
    flush();
}

void
SettingsManager::flush()
{
    QWriteLocker    wl( &m_rwLock );
    m_tmpXmlSettings.clear();
    m_tmpClassicSettings.clear();
}

SettingsManager::SettingsManager()
    : m_classicSettings(),
    m_xmlSettings(),
    m_tmpClassicSettings(),
    m_tmpXmlSettings(),
    m_rwLock()
{
}

SettingsManager::~SettingsManager()
{
}
