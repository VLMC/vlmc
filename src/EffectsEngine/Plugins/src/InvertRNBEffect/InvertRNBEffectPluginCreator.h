/*****************************************************************************
 * InvertRNBEffectPluginCreator.h: this class is used to instantiate
 *                                   an InvertRNBEffectPlugin
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

#ifndef INVERTRNBEFFECTPLUGINCREATOR_H_
#define INVERTRNBEFFECTPLUGINCREATOR_H_

#include <QObject>
#include <QtPlugin>
#include "IEffectPluginCreator.h"
#include "IEffectPlugin.h"
#include "InvertRNBEffectPlugin.h"

class   InvertRNBEffectPluginCreator : public QObject, public IEffectPluginCreator
{
    Q_OBJECT;
    Q_INTERFACES( IEffectPluginCreator );
    Q_CLASSINFO( "PLUGINNAME", "InvertRNB" );
public:
    IEffectPlugin*      createIEffectPluginInstance( void );
};

#endif // INVERTRNBEFFECTPLUGINCREATOR_H_
