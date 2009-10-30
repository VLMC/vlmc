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

char const *	MixerEffect::m_videoInputsNames[] =
  {
    "track0",
    "track1",
    "track2",
    "track3",
    "track4",
    "track5",
    "track6",
    "track7",
    "track8",
    "track9",
    "track10",
    "track11",
    "track12",
    "track13",
    "track14",
    "track15",
    "track16",
    "track17",
    "track18",
    "track19",
    "track20",
    "track21",
    "track22",
    "track23",
    "track24",
    "track25",
    "track26",
    "track27",
    "track28",
    "track29",
    "track30",
    "track31",
    "track32",
    "track33",
    "track34",
    "track35",
    "track36",
    "track37",
    "track38",
    "track39",
    "track40",
    "track41",
    "track42",
    "track43",
    "track44",
    "track45",
    "track46",
    "track47",
    "track48",
    "track49",
    "track50",
    "track51",
    "track52",
    "track53",
    "track54",
    "track55",
    "track56",
    "track57",
    "track58",
    "track59",
    "track60",
    "track61",
    "track62",
    "track63"
  };
char const *	MixerEffect::m_videoOutputsNames[] = { "out" };

MixerEffect::MixerEffect() : GenericEffect(
					   MixerEffect::m_videoInputsNames, MixerEffect::m_nbVideoInputs,
					   MixerEffect::m_videoOutputsNames, MixerEffect::m_nbVideoOutputs
					   )
{
};

MixerEffect::~MixerEffect()
{
}

void	MixerEffect::render( void )
{
  qint32	i;

  for ( i = 63; i >= 0; --i )
  {
      const LightVideoFrame&   lvf = m_videoInputs[i];
      if ( lvf->frame.octets != NULL )
      {
	m_videoOutputs[0] << lvf;
	return ;
      }
  }
  return ;
}
