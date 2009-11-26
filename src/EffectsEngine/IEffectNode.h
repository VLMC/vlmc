/*****************************************************************************
 * IGenericEffect.h: Interface of GenericEffect passed to init method of
 * of an IEffectPlugin
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

#ifndef IGENERICEFFECT_H_
#define IGENERICEFFECT_H_

#include "InSlot.hpp"
#include "OutSlot.hpp"
#include "LightVideoFrame.h"

class	IEffectNode
{
    public:

    virtual ~IEffectNode() {};
    virtual void                                init( quint32 const nbvideoinputs, quint32 const nbvideooutputs ) = 0;
    virtual InSlot<LightVideoFrame> &           getVideoInput(quint32 id) = 0;
    virtual OutSlot<LightVideoFrame> &          getVideoOutput(quint32 id) = 0;
};

#endif // IGENERICEFFECT_H_
