/*****************************************************************************
 * SettingsManager.cpp: Backend settings manager
 *****************************************************************************
 * Copyright (C) 2008-2009 VideoLAN
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
#include <QRegExp>
#include <QStringList>

#include <QtDebug>

void
SettingsManager::setValue( const QString &key,
                           const QVariant &value,
                           SettingsManager::Type type )
{
    if ( type == Project )
        m_tmpXmlSettings.insert( key, new SettingValue( value ) );
    else if ( type == Vlmc )
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
    if ( type == Project )
        settMap = &m_xmlSettings;
    else if ( type == Vlmc )
    {
        QSettings    sett;
        sett.setValue( key, value );
        sett.sync();
        settMap = &m_classicSettings;
    }
    if ( settMap->contains( key ) )
    {
        settMap->value( key )->set( value );
    }
    else
    {
        settMap->insert( key, new SettingValue( value ) );
    }

    return ;
}

QVariant
SettingsManager::value( const QString &key,
                        const QVariant &defaultValue,
                        SettingsManager::Type type )
{
    QReadLocker rl( &m_rwLock );

    if ( ( type == Project || type == All ) )
    {
        if ( m_xmlSettings.contains( key ) )
            return m_xmlSettings.value( key )->get();
        else
        {
            m_xmlSettings.insert( key, new SettingValue( defaultValue ) );
            return defaultValue;
        }
    }
    else if ( type == Vlmc )
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
}

QHash<QString, QVariant>
SettingsManager::group( const QString &groupName, SettingsManager::Type type )
{
    QHash<QString, QVariant>    ret;
    QReadLocker rl( &m_rwLock );
    if ( ( type == Project || type == All ) )
    {
         SettingHash::const_iterator it = m_xmlSettings.begin();
         SettingHash::const_iterator ed = m_xmlSettings.end();

         for ( ; it != ed; ++it )
         {
             if ( it.key().contains( QRegExp( "^" + groupName + "/" ) ) )
                 ret.insert( it.key().right( it.key().size()
                             - it.key().indexOf( "/" ) - 1 ), it.value()->get() );
         }
    }
    if ( type == Vlmc || type == All )
    {
         SettingHash::const_iterator it = m_classicSettings.begin();
         SettingHash::const_iterator ed = m_classicSettings.end();

         for ( ; it != ed; ++it )
         {
             if ( it.key().contains( QRegExp( QString("^").append( groupName ).append( "/" ) ) ) )
             {
                 ret.insert( it.key().right( it.key().size()
                             - it.key().indexOf( "/" ) - 1 ), it.value()->get() );
             }
         }

         QSettings sett;
         QStringList keys = sett.allKeys();

         foreach ( QString key, keys )
         {
             QString match("^");
             match.append( groupName ).append( "/" );
             QRegExp    exp( match );
             if ( key.contains( exp ) )
             {
                 ret.insert( key.right( key.size()
                             - key.indexOf( "/" ) - 1 ), sett.value( key ) );
                 if ( !m_classicSettings.contains( key ) )
                     m_classicSettings.insert( key, new SettingValue( sett.value( key ) ) );
             }
         }
    }
    return ret;
}

bool
SettingsManager::watchValue( const QString &key,
                             QObject* receiver,
                             const char *method,
                             SettingsManager::Type type,
                             Qt::ConnectionType cType )
{
    QReadLocker rl( &m_rwLock );

    if ( ( type == Project || type == All ) && m_xmlSettings.contains( key ) )
    {
        connect( m_xmlSettings[key], SIGNAL( changed( const QVariant& ) ),
                 receiver, method );
        return true;
    }
    else if ( ( type == Vlmc || type == All ) )
    {
        if ( m_classicSettings.contains( key ) )
        {
            connect( m_classicSettings[key], SIGNAL( changed( const QVariant& ) ),
                    receiver, method, cType );
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
SettingsManager::save( QDomDocument &xmlfile, QDomElement &root ) const
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
            QString name = key.right( key.size() - idx - 1 );

            parts.insert( part, settingPair( name, it.value() ) );
        }
    }
    QList<QString>  keys = parts.uniqueKeys();
    foreach( QString xmlKey, keys )
    {
        QDomElement node = xmlfile.createElement( xmlKey );
        QList<settingPair>  pairs = parts.values( xmlKey );
        foreach( settingPair pair, pairs )
        {
            QDomElement item = xmlfile.createElement( pair.first );
            item.setAttribute( "value", pair.second->get().toString() );
            node.appendChild( item );
        }
        root.appendChild( node );
    }
}

bool
SettingsManager::load( const QDomElement &element )
{
    //For now it only handle a project node.
    if ( element.isNull() == true || element.tagName() != "project" )
    {
        qWarning() << "Invalid settings node";
        return false ;
    }
    QWriteLocker    wLock( &m_rwLock );
    QDomNodeList list = element.childNodes();
    int nbChild = list.size();

     for ( int idx = 0; idx < nbChild; ++idx )
    {
        QDomNamedNodeMap attrMap = list.at( idx ).attributes();
        if ( attrMap.count() > 1 )
        {
            qWarning() << "Invalid number of attributes for" << list.at( idx ).nodeName();
            continue ;
        }
        QString key = "project/" + list.at( idx ).toElement().tagName();
        if ( m_xmlSettings.contains( key ) )
            m_xmlSettings[key]->set( QVariant( attrMap.item( 0 ).nodeValue() ) );
        else
            m_xmlSettings.insert( key,
                                  new SettingValue( QVariant( attrMap.item( 0 ).nodeValue() ) ) );
    }
     return true;
}

bool
SettingsManager::commit( SettingsManager::Type type )
{
    {
        QWriteLocker    wlock( &m_rwLock );
        if ( type == Project || type == All )
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
        if ( type == Vlmc || type == All )
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
