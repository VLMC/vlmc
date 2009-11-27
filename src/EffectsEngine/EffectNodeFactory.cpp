/*****************************************************************************
 * EffectNodeFactory.cpp: this class is used to instantiate a new EffectNode
 *                        which contains builtin or plugin effect
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

#include "EffectNodeFactory.h"
#include <QDir>

#define PLUGINS_PATH "./src/EffectsEngine/Plugins"

EffectNodeFactory::EffectNodeFactory()
{
    QDir        dir;
    QStringList filter;
    quint32     i;
    quint32     size;
    QFileInfoList list;

    if (dir.cd(PLUGINS_PATH) == false)
        qDebug() << "ERROR PLUGINS_PATH";
    else
    {
        filter << "*.so";
        list = dir.entryInfoList(filter, QDir::Files);
        size = list.size();
        if (size == 0)
            qDebug() << "AUCUN PLUGINS";
        else
            for (i = 0; i < size; ++i)
            {
                qDebug() << list.at(i).fileName();
            }
    }
    m_epc["mixer"] = new MixerEffectPluginCreator();
    m_epc["green"] = new GreenFilterEffectPluginCreator();
}

EffectNodeFactory::~EffectNodeFactory()
{
}

EffectNode*        EffectNodeFactory::getEffect( quint32 id )
{
    if ( id == 1 )
        return ( new EffectNode( m_epc["mixer"]->getIEffectPlugin() ) );
    else if ( id == 2 )
        return ( new EffectNode( m_epc["green"]->getIEffectPlugin() ) );
}
