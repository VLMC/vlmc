/*****************************************************************************
 * GenericEffect.h: Abstract class you must inherit from, when you program
 * an effect module
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

#ifndef GENERICEFFECT_H_
#define GENERICEFFECT_H_

#include <QtGlobal>
#include <QString>
#include <QHash>

#include "InSlot.hpp"
#include "OutSlot.hpp"
#include "LightVideoFrame.h"

class	GenericEffect
{

 public:
 
 // CTOR & DTOR

  virtual ~GenericEffect();

 protected:

/*   GenericEffect( bool dynamicsvideoinputs, */
/* 		 char const * videoinputs[], */
/* 		 quint32 const nbvideoinputs, */
/* 		 bool dynamicsvideooutputs, */
/* 		 char const * videooutputs[], */
/* 		 quint32 const nbvideooutputs ); */

  GenericEffect( char const * videoinputs[],
		 quint32 const nbvideoinputs,
		 char const * videooutputs[],
		 quint32 const nbvideooutputs );

 public:

  // RENDER METHOD

  virtual void  render(void) = 0;

  // CONNECTION METHODS


 public:

  //
  // CONNECTIONS BETWEEN GENERICEFFECTS
  //

/*   void				connectDynOutToStatIn( GenericEffect* destinationeffect, QString const & inslotname ); */
/*   void				connectDynOutToDynIn( GenericEffect* destinationeffect ); */
  void					connectOutput( quint32 outIndex, GenericEffect* destEffect, quint32 inIndex );
/*   void				connectStatOutToDynIn( QString const & outslotname, GenericEffect* destinationeffect ); */

  //
  // CONNECTIONS BETWEEN GENERICEFFECT & OUTSLOTS/INSLOTS
  //

  void					connect( OutSlot<LightVideoFrame> & out, quint32 inIndex );
  void					connect( quint32 outIndex, InSlot<LightVideoFrame> & in);

 private:

  //

/*   bool				areThereDynamicsVideoInputs( void ); */
/*   bool				areThereDynamicsVideoOutputs( void ); */
/*   InSlot<LightVideoFrame>&	getStaticVideoInput( QString const & name ); */
/*   OutSlot<LightVideoFrame>&	getStaticVideoOutput( QString const & name ); */
/*   InSlot<LightVideoFrame>&	getDynamicVideoInput( void ); */
/*   OutSlot<LightVideoFrame>&	getDynamicVideoOutput( void ); */

 private:

/*   bool						m_dynamicsVideoInputs; */
/*   bool						m_dynamicsVideoOutputs; */

 protected:

  //  QString					m_name;
  InSlot<LightVideoFrame>*			m_videoInputs;
  OutSlot<LightVideoFrame>*			m_videoOutputs;
/*   QHash< QString, InSlot<SoundSampleFrame> >	m_soundInputs; */
/*   QHash< QString, OutSlot<SoundSampleFrame> >m_soundOutputs; */
/*   QHash< QString, InSlot<LightVideoFrame> >	m_controlInputs; */
/*   QHash< QString, OutSlot<LightVideoFrame> >	m_controlOutputs; */
  
};

#endif // GENERICEFFECT_H_
