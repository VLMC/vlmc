/*****************************************************************************
 * BlitInRectangleEffectPlugin.h: blit src video input in a rectangle on dst
 *                                video input and output the result
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

#ifndef BLITINRECTANGLEEFFECTPLUGIN_H_
#define BLITINRECTANGLEEFFECTPLUGIN_H_

#include <QImage>
#include <QPainter>
#include "IEffectNode.h"
#include "IEffectPlugin.h"

class	BlitInRectangleEffectPlugin : public QObject, public IEffectPlugin
{
 public:

  // CTOR & DTOR

  BlitInRectangleEffectPlugin();
  ~BlitInRectangleEffectPlugin();

  // INIT

  void  init( IEffectNode* ien );

  // RENDER METHOD

  void	render( void );

 private:

  IEffectNode*                  m_ien;
};

#endif // BLITINRECTANGLEEFFECTPLUGIN_H_
