/*****************************************************************************
 * EffectPluginTypeManager.cpp: this class is used to instantiate one
 *                              EffectPluginTypeLoader by .so
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
 *
 * Authors: Vincent Carrubba <cyberbouba@gmail.com>
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

#include "EffectPluginTypeManager.h"
#include "EffectPluginTypeLoader.h"
#include "config.h"

#include <QDebug>
#include <QDir>
#include <QStringList>

EffectPluginTypeManager::EffectPluginTypeManager( void ) : m_higherFreeId( 2 )
{
    QStringList dirs;
    qint32      plugins = 0;

    dirs << "bin/effects/";

#ifdef VLMC_EFFECTS_DIR
    dirs << VLMC_EFFECTS_DIR;
#endif

    foreach ( QString path, dirs )
        plugins += loadPlugins( path );

    if ( plugins == 0 )
        qWarning() << "No plugins found!";
}

qint32
EffectPluginTypeManager::loadPlugins( const QString &path )
{
    QDir                        dir;
    quint32                     i;
    quint32                     size;
    qint32                      pluginsLoaded = 0;
    QFileInfoList               list;
    EffectPluginTypeLoader*     tmpEptl = NULL;

    qDebug() << "Searching for effects in" << path;

    if ( dir.cd( path ) )
    {
        list = dir.entryInfoList( QDir::Files );
        size = list.size();
        if ( !list.empty() )
            for ( i = 0; i < size; ++i )
            {
                if ( !QLibrary::isLibrary( list.at( i ).absoluteFilePath() ) )
                    continue;

                if ( !tmpEptl )
                    tmpEptl = new EffectPluginTypeLoader();

                if ( tmpEptl->load( list.at( i ).absoluteFilePath() ) == true )
                {
                    m_eptlByName[ tmpEptl->pluginName() ] = tmpEptl;
                    m_eptlById[ m_higherFreeId ] = tmpEptl;
                    m_nameById[ m_higherFreeId ] = tmpEptl->pluginName();
                    ++m_higherFreeId;
                    qDebug() << list.at( i ).fileName() << "loaded.";
                    tmpEptl = NULL;
                    pluginsLoaded++;
                }
                else
                    qWarning() << list.at( i ).fileName() << "is not a valid plugin.";
            }

        if ( tmpEptl )
            delete tmpEptl;
    }
    return pluginsLoaded;
}

EffectPluginTypeManager::~EffectPluginTypeManager()
{
    if ( m_eptlById.empty() == false )
    {
        quint32     i;
        quint32     size;

        size = m_eptlById.size();
        for ( i = 0; i < size; ++i )
            delete m_eptlById[i];
    }
}

IEffectPlugin*
EffectPluginTypeManager::createIEffectPluginInstance( quint32 typeId ) const
{
    QMap<quint32, EffectPluginTypeLoader*>::const_iterator it = m_eptlById.find( typeId );

    if ( it != m_eptlById.end() )
        return it.value()->createIEffectPluginInstance();
    return NULL;
}

IEffectPlugin*
EffectPluginTypeManager::createIEffectPluginInstance( const QString & typeName ) const
{
    QMap<QString, EffectPluginTypeLoader*>::const_iterator it = m_eptlByName.find( typeName );

    if ( it != m_eptlByName.end() )
        return it.value()->createIEffectPluginInstance();
    return NULL;
}

QList<QString>
EffectPluginTypeManager::getEffectPluginTypesNamesList( void ) const
{
    return ( m_nameById.values() );
}

QList<quint32>
EffectPluginTypeManager::getEffectPluginTypesIdsList( void ) const
{
    return m_nameById.keys();
}

const QString
EffectPluginTypeManager::getEffectPluginTypeNameByTypeId( quint32 typeId ) const
{
    return m_nameById.value( typeId, "" );
}

quint32
EffectPluginTypeManager::getEffectPluginTypeIdByTypeName( const QString& typeName ) const
{
    return m_nameById.key( typeName, 0 );
}
