/*****************************************************************************
 * InvertRNBEffectPlugin.cpp: invert red and blue picture composantes
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

#include "InvertRNBEffectPlugin.h"
#include <QtDebug>

InvertRNBEffectPlugin::InvertRNBEffectPlugin()
{
}

InvertRNBEffectPlugin::~InvertRNBEffectPlugin()
{
}

void            InvertRNBEffectPlugin::init(IEffectNode* ien)
{
    m_ien = ien;
    m_ien->createStaticVideoInput();
    m_ien->createStaticVideoOutput();
    return ;
}

void    InvertRNBEffectPlugin::render( void )
{
    quint32		i;
    LightVideoFrame	tmp;
    quint8              tmpay;

    (*m_ien->getStaticVideoInput(1)) >> tmp;
    if (tmp->frame.octets != NULL)
    {
        for ( i = 0; i < tmp->nbpixels; ++i )
        {
            tmpay = tmp->frame.pixels[i].Red;
            tmp->frame.pixels[i].Red = tmp->frame.pixels[i].Blue;
            tmp->frame.pixels[i].Blue = tmpay;
        }
        (*m_ien->getStaticVideoOutput(1)) << tmp;
    }
    return ;
}
