/*****************************************************************************
 * EffectsEngine.h: Main class of the effects engine
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

#ifndef EFFECTSENGINE_H_
#define EFFECTSENGINE_H_

#include <QtGlobal>
#include <QHash>
#include <iostream>
#include <QReadWriteLock>

#include "LightVideoFrame.h"
#include "InSlot.hpp"
#include "OutSlot.hpp"
#include "EffectNodeFactory.h"

class	EffectsEngine
{

 public:

  // CTOR & DTOR

  EffectsEngine( void
		/* quint32 nbinputs, quint32 nboutputs  */);
  ~EffectsEngine();


  EffectNode*        operator->( void );
  EffectNode const * operator->( void ) const;
  EffectNode*        operator*( void );
  EffectNode const * operator*( void ) const;

  void               enable( void );
  void               disable( void );

  const LightVideoFrame & getVideoOutput( quint32 outId ) const;
  void                    render( void );
  void                    setVideoInput( quint32 inId, const LightVideoFrame & frame );

 private:

  mutable QReadWriteLock                        m_rwl;
  EffectNodeFactory                             m_enf;
  EffectNode*                                   m_patch;
  EffectNode*                                   m_bypassPatch;
  bool                                          m_enabled;
  bool                                          m_processedInBypassPatch;
};

#endif // EFFECTSENGINE_H_
