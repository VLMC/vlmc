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
#include <QObject>

#include "InSlot.hpp"
#include "OutSlot.hpp"
#include "LightVideoFrame.h"

class	GenericEffect : public QObject
{

  Q_OBJECT
  Q_CLASSINFO("authors", "Vincent Carrubba")
  Q_CLASSINFO("mail", "boubak@vlmc.org")
  Q_CLASSINFO("name", "GenericEffect")
  Q_CLASSINFO("category", "Primtives")
  Q_CLASSINFO("www", "www.vlmc.org")
  Q_CLASSINFO("version", "0.1")
  Q_CLASSINFO("description", "This is the generic effect your effect must inherit from to work into the effects engine. It's the effects container too.")

    public:
 
 // CTOR & DTOR

  virtual ~GenericEffect();

  GenericEffect( char const * videoinputs[],
		 quint32 const nbvideoinputs,
		 char const * videooutputs[],
		 quint32 const nbvideooutputs );

  GenericEffect();

  //-------------------------------------------------------------------
  //
  // EFFECT CONFIGURATION
  //
  //

  //
  // SETTING EFFECT CONFIGURATION
  //

  // STATICS SLOTS

  bool		addStaticVideoInput( QByteArray const & name );
  bool		addStaticVideoOutput( QByteArray const & name );
  bool		addStaticAudioInput( QByteArray const & name );
  bool		addStaticAudioOutput(QByteArray const & name );
  bool		addStaticControlInput(QByteArray const & name );
  bool		addStaticControlOutput(QByteArray const & name );

  bool		removeStaticVideoInput( QByteArray const & name );
  bool		removeStaticVideoOutput( QByteArray const & name );
  bool		removeStaticAudioInput( QByteArray const & name );
  bool		removeStaticAudioOutput( QByteArray const & name );
  bool		removeStaticControlInput( QByteArray const & name );
  bool		removeStaticControlOutput( QByteArray const & name );

  // DYNAMICS SLOTS

  void		enableDynamicVideoInput( void );
  void		enableDynamicVideoOutput( void );
  void		enableDynamicAudioInput( void );
  void		enableDynamicAudioOutput( void );
  void		enableDynamicControlInput( void );
  void		enableDynamicControlOutput( void );

  void		disableDynamicVideoInput( void );
  void		disableDynamicVideoOutput( void );
  void		disableDynamicAudioInput( void );
  void		disableDynamicAudioOutput( void );
  void		disableDynamicControlInput( void );
  void		disableDynamicControlOutput( void );

  //
  // GETTING EFFECT CONFIGURATION
  //

  // STATICS SLOTS


  //  QList<QByteArray>	getStati

  quint32	getNBStaticVideosInputs( void ) const;
  quint32	getNBStaticVideosOutputs( void ) const;
  quint32	getNBStaticAudiosIntputs( void ) const;
  quint32	getNBStaticAudiosOutputs( void ) const;
  quint32	getNBStaticControlsInputs( void ) const;
  quint32	getNBStaticControlsOutputs( void ) const;

  // DYNAMICS SLOTS

  quint32	getNBDynamicVideosInputs( void ) const;
  quint32	getNBDynamicVideosOutputs( void ) const;
  quint32	getNBDynamicAudiosIntputs( void ) const;
  quint32	getNBDynamicAudiosOutputs( void ) const;
  quint32	getNBDynamicControlsInputs( void ) const;
  quint32	getNBDynamicControlsOutputs( void ) const;

  bool		areDynamicVideosInputsEnabled( void ) const;
  bool		areDynamicVideosOutputsEnabled( void ) const;
  bool		areDynamicAudiosIntputsEnabled( void ) const;
  bool		areDynamicAudiosOutputsEnabled( void ) const;
  bool		areDynamicControlsInputsEnabled( void ) const;
  bool		areDynamicControlsOutputsEnabled( void ) const;

  //-------------------------------------------------------------------
  //
  // GETTING EFFECT WIDGET
  //
  //

  QWidget*	getWidget( void );

  //-------------------------------------------------------------------
  //
  // RENDER METHOD
  //
  //

  virtual void  render( void );



  //-------------------------------------------------------------------
  //
  // MISSING FUNCTIONNALITY
  //
  //
  
  // IN PRIVATE : GETTING SLOT FOR THE EFFECT DEVELOPPER

  // TO FINISH : CONNECTION AND BINDINDG

  // INTERPRETER AND ITS PRIMITIVE METHODES

  // REPLACE VIRTUAL PURE RENDER METHOD BY VIRTUAL RENDER METHOD WHO RENTER THE SUB-EFFECTS BY DEFAULT
  // WITH THE MULTI-THREAD RENDER CLASS

  // EffectsFactory static instance into GenericEffect

  // SUB-EFFECTS MANAGEMENT METHODS

  //-------------------------------------------------------------------
  //
  // CONNECTION AND BINDING METHODS
  //
  //

  

  //-------------------------------------------------------------------
  //
  // DEPRECATED
  //
  //

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

 protected:


  InSlot<LightVideoFrame>*			m_videoInputs;
  OutSlot<LightVideoFrame>*			m_videoOutputs;

  //-------------------------------------------------------------------
  // SLOTS
  //

  // STATIC SLOTS

  QMap< quint32, InSlot<LightVideoFrame> >	m_staticVideosInputs;
  QMap< quint32, OutSlot<LightVideoFrame> >	m_staticVideosOutputs;
  // QMap< quint32, InSlot<> >			m_staticAudiosInputs;
  // QMap< quint32, OutSlot<> >			m_staticAudiosOutputs;
  QMap< quint32, InSlot<qreal> >		m_staticControlsInputs;
  QMap< quint32, OutSlot<qreal> >		m_staticControlsOutputs;

  // DYNAMIC SLOTS

  bool						m_enableDynamicVideosInputs;
  bool						m_enableDynamicVideosOutputs;
  bool						m_enableDynamicAudiosInputs;
  bool						m_enableDynamicAudiossOutputs;
  bool						m_enableDynamicControlInputs;
  bool						m_enableDynamicControlOutputs;

  QList< InSlot<LightVideoFrame> >		m_dynamicVideosInputs;
  QList< OutSlot<LightVideoFrame> >		m_dynamicVideosOutputs;
  // QList< InSlot<> >				m_dynamicAudiosInputs;
  // QList< OutSlot<> >				m_dynamicAudiosOutputs;
  QList< InSlot<qreal> >			m_dynamicControlsInputs;
  QList< OutSlot<qreal> >			m_dynamicControlsOutputs;
  
};

#endif // GENERICEFFECT_H_
