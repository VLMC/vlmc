/*****************************************************************************
 * MixerEffect.h: Effect module to mix multiple frame in one
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

#ifndef MIXEREFFECT_H_
#define MIXEREFFECT_H_

#include "GenericEffect.h"

class	MixerEffect : public GenericEffect
{
public:

  // CTOR & DTOR

  MixerEffect();
  ~MixerEffect();

  // RENDER METHOD

  void	render( void );

private:

  static	quint32 const	m_nbVideoInputs = 64;
  static	quint32 const	m_nbVideoOutputs = 64;
  static	char const *	m_videoInputsNames[m_nbVideoInputs];
  static	char const *	m_videoOutputsNames[m_nbVideoOutputs];

};

#endif // MIXEREFFECT_H_
