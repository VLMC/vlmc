/*****************************************************************************
 * MixerEffectPlugin.cpp: Effect module to mix multiple frame in one
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

#include "MixerEffectPlugin.h"
#include <QtDebug>

MixerEffectPlugin::MixerEffectPlugin()
{
}

MixerEffectPlugin::~MixerEffectPlugin()
{
}

void    MixerEffectPlugin::init( IEffectNode* ien )
{
    m_ien = ien;
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoOutput();
    return ;
}

void	MixerEffectPlugin::render( void )
{
  quint32	i;
  quint32       nbIns;

  nbIns = m_ien->getNBStaticsVideosInputs();
  for ( i = 1; i <= nbIns; ++i )
  {
      const LightVideoFrame&   lvf = (*m_ien->getStaticVideoInput( i ));
      if ( lvf->frame.octets != NULL )
      {
          (*m_ien->getStaticVideoOutput( 1 )) << lvf;
          return ;
      }
  }
  return ;
}
