/*****************************************************************************
 * EffectsFactory.h: this class is used to instantiate a new GenericEffect
 *                   which contains builtin or plugin effect
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

#ifndef EFFECTSFACTORY_H_
#define EFFECTSFACTORY_H_

#include <QMap>

#include "GreenFilterEffectFactory.h"
#include "MixerEffectFactory.h"

#include "IEffectPluginFactory.h"
#include "EffectNode.h"

/* class   SmartFactoryProxy */
/* { */

/* }; */

class	EffectsFactory
{
 public:

  // CTOR & DTOR

    EffectsFactory();
    ~EffectsFactory();

  EffectNode*        getEffect( quint32 id );
 private:
  QMap<QByteArray, IEffectPluginFactory*>       m_epf;
};

#endif // EFFECTSFACTORY_H_
