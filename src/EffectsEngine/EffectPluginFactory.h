/*****************************************************************************
 * EffectPluginFactory.h: this class is used to load a .so and instantiate
 *                          a new IEffectPlugin from this
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

#ifndef EFFECTPLUGINFACTORY_H_
#define EFFECTPLUGINFACTORY_H_

#include <QPluginLoader>
#include <QList>
#include <IEffectPluginCreator.h>
#include <IEffectPlugin.h>

class   EffectPluginFactory
{
public:

    EffectPluginFactory();
    ~EffectPluginFactory();

    bool                load( QString const & fileName );
    IEffectPlugin*      newIEffectPlugin( void );
    bool                deleteIEffectPluginInstance( quint32 id )

private:

    QPluginLoader                       m_qpl;
    IEffectPluginCreator*               m_iepc;
    QMap<quint32, IEffectPlugin*>       m_iepi;
    quint32                             m_higherFreeId;
    quint32                             m_mapHoles;
};

// L'IEffectPluginCreator doit aussi avoir la methode deleteInstance( quint32 id )
// L'IEffectPlugin doit avoir son constructeur et son destructeur en private avec IEffectPluginCreator en friend

#endif // EFFECTPLUGINFACTORY_H_
