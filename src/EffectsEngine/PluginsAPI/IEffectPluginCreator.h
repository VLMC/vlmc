/*****************************************************************************
 * IEffectPluginCreator.cpp: Interface that must inherit the class who can
 *                           instantiate an effect plugin
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

#ifndef IEFFECTPLUGINFACTORY_H_
#define IEFFECTPLUGINFACTORY_H_

#include <QtPlugin>

class   IEffectPlugin;

class   IEffectPluginCreator
{
public:
    virtual ~IEffectPluginCreator() {};
    virtual IEffectPlugin*      createIEffectPluginInstance( void ) = 0;
};

Q_DECLARE_INTERFACE(IEffectPluginCreator, "IEffectPluginCreator/0.1")

#endif // IEFFECTPLUGINFACTORY_H_
