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

  // MAIN METHOD

  void		render( void );

  // BYPASSING

  void		enable( void );
  void		disable( void );

  // INPUTS & OUTPUTS METHODS

/*   void			setClock( Parameter currentframenumber ); */
  void				setInputFrame( LightVideoFrame& frame, quint32 tracknumber );
  LightVideoFrame const &       getOutputFrame( quint32 tracknumber ) const;

 private:

  // START & STOP

  void		start( void );
  void		stop( void );

  // EFFECTS LOADING & UNLOADING


  void		loadEffects( void );
  void		unloadEffects( void );

  // EFFECTS PATCHING

  void		patchEffects( void );


 private:

  EffectNodeFactory                             m_enf;
  QHash< quint32, EffectNode* >                 m_effects;
  OutSlot<LightVideoFrame>*			m_videoInputs; // It's OutSlots because, it's the Outputs of the workflow, that should be connected to InSlots of effects
  InSlot<LightVideoFrame>*			m_videoOutputs; // It's InSlots because, it's the Inputs of the effect engine, that should be connected to OutSlots of the renderer
/*   OutSlot<LightParameter>			m_clockInput;	 // It's OutSlots because, it's the Outputs of the clock of the workflow, that should be connected to OutSlots */

  QReadWriteLock*                               m_inputLock;

};

#endif // EFFECTSENGINE_H_
