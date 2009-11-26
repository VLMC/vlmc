/*****************************************************************************
 * EffectsFactory.cpp: this class is used to instantiate a new GenericEffect
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

#include "EffectsFactory.h"



EffectsFactory::EffectsFactory()
{
    m_epf["mixer"] = new MixerEffectFactory();
    m_epf["green"] = new GreenFilterEffectFactory();
}

EffectsFactory::~EffectsFactory()
{
}

EffectNode*        EffectsFactory::getEffect( quint32 id )
{
    if ( id == 1 )
        return ( new EffectNode( m_epf["mixer"]->getIEffectPlugin() ) );
    else if ( id == 2 )
        return ( new EffectNode( m_epf["green"]->getIEffectPlugin() ) );
}
