/*****************************************************************************
 * EffectPluginTypeManager.h: this class is used to instantiate one
 *                              EffectPluginTypeLoader by .so
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
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

#ifndef EFFECTPLUGINTYPEMANAGER_H_
#define EFFECTPLUGINTYPEMANAGER_H_

#define PLUGINS_PATH "./src/EffectsEngine/Plugins/lib/"

#include <QMap>

class   EffectPluginTypeLoader;
class   IEffectPlugin;

class   EffectPluginTypeManager
{

 public:

    EffectPluginTypeManager( void );
    ~EffectPluginTypeManager();

    IEffectPlugin*      createIEffectPluginInstance( quint32 typeId ) const;
    IEffectPlugin*      createIEffectPluginInstance( const QString & typeName ) const;

    QList<QString>      getEffectPluginTypesNamesList( void ) const;
    QList<quint32>      getEffectPluginTypesIdsList( void ) const;

    const QString       getEffectPluginTypeNameByTypeId( quint32 typeId ) const;
    quint32             getEffectPluginTypeIdByTypeName( const QString & typeName ) const;

 private:

    QMap<QString, EffectPluginTypeLoader*>      m_eptlByName;
    QMap<quint32, EffectPluginTypeLoader*>      m_eptlById;
    QMap<quint32, QString>                      m_nameById;
    quint32                                     m_higherFreeId;

};

#endif // EFFECTPLUGINTYPEMANAGER_H_
