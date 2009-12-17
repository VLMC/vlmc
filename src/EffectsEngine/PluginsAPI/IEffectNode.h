/*****************************************************************************
 * IEffectNode.h: Interface of EffectNode passed to IEffectPlugin constructor
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

#ifndef IEFFECTNODE_H_
#define IEFFECTNODE_H_

#include "InSlot.hpp"
#include "OutSlot.hpp"
#include "LightVideoFrame.h"

class	IEffectNode
{
    public:

    virtual ~IEffectNode() {};
    virtual void                                init( quint32 const nbvideoinputs, quint32 const nbvideooutputs ) = 0;
    virtual InSlot<LightVideoFrame> &           getVideoInput(quint32 id) = 0;
    virtual OutSlot<LightVideoFrame> &          getVideoOutput(quint32 id) = 0;

    virtual quint32             getTypeId( void ) const = 0;
    virtual QString const &     getTypeName( void ) const = 0;
    virtual quint32             getInstanceId( void ) const = 0;
    virtual QString const &     getInstanceName( void ) const = 0;

    // ================================================================= SLOTS ========================================================================

    //------------------------------------------------------------//
    //                 MANAGING STATICS SLOTS                     //
    //------------------------------------------------------------//

    // -------------- CREATE --------------

    virtual void		createStaticVideoInput( void ) = 0;
    virtual void		createStaticVideoOutput( void ) = 0;

    virtual void		createStaticVideoInput( QString const & name ) = 0;
    virtual void		createStaticVideoOutput( QString const & name ) = 0;

    // -------------- REMOVE --------------

    virtual bool		removeStaticVideoInput( QString const & name ) = 0;
    virtual bool		removeStaticVideoOutput( QString const & name ) = 0;

    virtual bool		removeStaticVideoInput( quint32 id ) = 0;
    virtual bool		removeStaticVideoOutput( quint32 id ) = 0;

    // -------------- GET --------------

    virtual InSlot<LightVideoFrame>*		getStaticVideoInput( QString const & name ) const = 0;
    virtual OutSlot<LightVideoFrame>*	        getStaticVideoOutput( QString const & name ) const = 0;

    virtual InSlot<LightVideoFrame>*		getStaticVideoInput( quint32 id ) const = 0;
    virtual OutSlot<LightVideoFrame>*		getStaticVideoOutput( quint32 id ) const = 0;

    virtual QList<InSlot<LightVideoFrame>*>		getStaticsVideosInputsList( void ) const = 0;
    virtual QList<OutSlot<LightVideoFrame>*>		getStaticsVideosOutputsList( void ) const = 0;

    // -------------- GET INFOS ON SLOTS --------------

    virtual QList<QString>	getStaticsVideosInputsNamesList( void ) const = 0;
    virtual QList<QString>	getStaticsVideosOutputsNamesList( void ) const = 0;

    virtual QList<quint32>	getStaticsVideosInputsIdsList( void ) const = 0;
    virtual QList<quint32>	getStaticsVideosOutputsIdsList( void ) const = 0;

    virtual QString const        getStaticVideoInputNameById( quint32 const id ) const = 0;
    virtual QString const        getStaticVideoOutputNameById( quint32 const id ) const = 0;

    virtual quint32              getStaticVideoInputIdByName( QString const & name ) const = 0;
    virtual quint32              getStaticVideoOutputIdByName( QString const & name ) const = 0;

    virtual quint32              getNBStaticsVideosInputs( void ) const = 0;
    virtual quint32              getNBStaticsVideosOutputs( void ) const = 0;

};

#endif // IEFFECTNODE_H_