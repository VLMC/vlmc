/*****************************************************************************
 * MixerEffect.cpp: Effect module to mix multiple frame in one
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

#include "MixerEffect.h"
#include <QtDebug>

MixerEffect::MixerEffect()
{
}

MixerEffect::~MixerEffect()
{
}

void    MixerEffect::init( IEffectNode* ien )
{
    m_ien = ien;
    m_ien->init(MixerEffect::m_nbVideoInputs, MixerEffect::m_nbVideoOutputs);
    return ;
}

void	MixerEffect::render( void )
{
  quint32	i;

  for ( i = 0; i < 64; ++i )
  {
      const LightVideoFrame&   lvf = m_ien->getVideoInput( i );
      if ( lvf->frame.octets != NULL )
      {
          m_ien->getVideoOutput( 0 ) << lvf;
          return ;
      }
  }
  return ;
}
