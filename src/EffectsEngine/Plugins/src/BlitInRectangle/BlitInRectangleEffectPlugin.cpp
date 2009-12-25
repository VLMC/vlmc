/*****************************************************************************
 * BlitInRectangleEffectPlugin.cpp: blit src video input in a rectangle on dst
 *                                   video input and output the result
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

#include "BlitInRectangleEffectPlugin.h"
#include <QtDebug>


BlitInRectangleEffectPlugin::BlitInRectangleEffectPlugin()
{
}

BlitInRectangleEffectPlugin::~BlitInRectangleEffectPlugin()
{
}

void            BlitInRectangleEffectPlugin::init(IEffectNode* ien)
{
    m_ien = ien;
    m_ien->createStaticVideoInput("src");
    m_ien->createStaticVideoInput("dst");
    m_ien->createStaticVideoOutput("aux");
    m_ien->createStaticVideoOutput("res");
    return ;
}

void    BlitInRectangleEffectPlugin::render( void )
{
    LightVideoFrame     lvf1;
    LightVideoFrame     lvf2;

    (*m_ien->getStaticVideoInput("src")) >> lvf1;
    (*m_ien->getStaticVideoInput("dst")) >> lvf2;

    QImage              src( lvf1->frame.octets, 640, 480, QImage::Format_RGB888 );
    QImage              dst( lvf2->frame.octets, 640, 480, QImage::Format_RGB888 );
    QPainter            p( &dst );

    p.drawImage( 100, 100, src);
    (*m_ien->getStaticVideoOutput("aux")) << lvf2;
    (*m_ien->getStaticVideoOutput("res")) << lvf2;
    return ;
}
