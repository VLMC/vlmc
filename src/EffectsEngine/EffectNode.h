/*****************************************************************************
 * EffectNode.h: Abstract class you must inherit from, when you program
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

#ifndef EFFECTNODE_H_
#define EFFECTNODE_H_

#include <QtGlobal>
#include <QString>
#include <QHash>
#include <QObject>
#include <QString>

#include "IEffectNode.h"
#include "IEffectPlugin.h"
#include "InSlot.hpp"
#include "OutSlot.hpp"
#include "LightVideoFrame.h"
#include "SemanticObjectManager.hpp"

template class SemanticObjectManager<InSlot<LightVideoFrame> >;
template class SemanticObjectManager<OutSlot<LightVideoFrame> >;

// template class SemanticObjectManager<InSlot<AudioSoundSample> >;
// template class SemanticObjectManager<OutSlot<AudioSoundSample> >;

// template class SemanticObjectManager<InSlot<qreal> >;
// template class SemanticObjectManager<OutSlot<qreal> >;

class	EffectNode : public IEffectNode
{

    ////
    ////
    ////
    ////
    //// METHODS
    ////
    ////
    ////
    ////

    public:

    //
    //
    //
    // CTOR & DTOR
    //
    //
    //

    virtual ~EffectNode();

    EffectNode(IEffectPlugin* plugin);

    //    EffectNode();

    void        init(quint32 const nbvideoinputs,
                     quint32 const nbvideooutputs);

    IEffectPlugin*       getInternalPlugin( void );

    //
    //
    //
    // EFFECT INSTANCE AND EFFECT TYPE INFORMATIONS
    //
    //
    //

    void        setTypeId( quint32 typeId );
    void        setTypeName( QString const & typeName );
    void        setInstanceId( quint32 instanceId );
    void        setInstanceName( QString const & instanceName );

    quint32             getTypeId( void ) const;
    QString const &     getTypeName( void ) const;
    quint32             getInstanceId( void ) const;
    QString const &     getInstanceName( void ) const;


    //
    //
    //
    // EFFECT CONFIGURATION
    //
    //
    //

    //
    //
    // SETTING EFFECT CONFIGURATION
    //
    //

    //
    // STATICS SLOTS
    //

    bool		addStaticVideoInput( QByteArray const & name );
    bool		addStaticVideoOutput( QByteArray const & name );
    //     bool		addStaticAudioInput( QByteArray const & name );
    //     bool		addStaticAudioOutput( QByteArray const & name );
    //     bool		addStaticControlInput( QByteArray const & name );
    //     bool		addStaticControlOutput( QByteArray const & name );

    bool		addStaticVideoInput( void );
    bool		addStaticVideoOutput( void );
    //     bool		addStaticAudioInput( void );
    //     bool		addStaticAudioOutput( void );
    //     bool		addStaticControlInput( void );
    //     bool		addStaticControlOutput( void );

    bool		removeStaticVideoInput( QByteArray const & name );
    bool		removeStaticVideoOutput( QByteArray const & name );
    //     bool		removeStaticAudioInput( QByteArray const & name );
    //     bool		removeStaticAudioOutput( QByteArray const & name );
    //     bool		removeStaticControlInput( QByteArray const & name );
    //     bool		removeStaticControlOutput( QByteArray const & name );

    bool		removeStaticVideoInput( quint32 id );
    bool		removeStaticVideoOutput( quint32 id );
    //     bool		removeStaticAudioInput( quint32 id );
    //     bool		removeStaticAudioOutput( quint32 id );
    //     bool		removeStaticControlInput( quint32 id );
    //     bool		removeStaticControlOutput( quint32 id );

    //
    // DYNAMICS SLOTS
    //

//     void		enableDynamicVideoInput( void );
//     void		enableDynamicVideoOutput( void );
//     void		enableDynamicAudioInput( void );
//     void		enableDynamicAudioOutput( void );
//     void		enableDynamicControlInput( void );
//     void		enableDynamicControlOutput( void );

//     void		disableDynamicVideoInput( void );
//     void		disableDynamicVideoOutput( void );
//     void		disableDynamicAudioInput( void );
//     void		disableDynamicAudioOutput( void );
//     void		disableDynamicControlInput( void );
//     void		disableDynamicControlOutput( void );

//
//
// GETTING SLOTS
//
//

//
// STATIC SLOTS
//

    InSlot<LightVideoFrame>*		getStaticVideoInput( QByteArray const & name ) const;
    OutSlot<LightVideoFrame>*	        getStaticVideoOutput( QByteArray const & name ) const;
    //     InSlot<AudioSoundSample>*		getStaticAudioInput( QByteArray const & name ) const;
    //     OutSlot<AudioSoundSample>*		getStaticAudioOutput( QByteArray const & name ) const;
    //     InSlot<qreal>*		getStaticControlInput( QByteArray const & name ) const;
    //     OutSlot<qreal>* 		getStaticControlOutput( QByteArray const & name ) const;

    IntSlot<LightVideoFrame>*		getStaticVideoInput( quint32 id ) const;
    OutSlot<LightVideoFrame>*		getStaticVideoOutput( quint32 id ) const;
    //  InSlot<AudioSoundSample>*		getStaticAudioInput( quint32 id ) const;
    //  OutSlot<AudioSoundSample>*		getStaticAudioOutput( quint32 id ) const;
    //  InSlot<qreal>*		getStaticControlInput( quint32 id ) const;
    //  OutSlot<qreal>*		getStaticControlOutput( quint32 id ) const;

    QList<OutSlot<LightVideoFrame>*>		getStaticVideoInputList( void ) const;
    QList<InSlot<LightVideoFrame>*>		getStaticVideoOutputList( void ) const;
    //  QList<InSlot<AudioSoundSample>*>		getStaticAudioInputList( void ) const;
    //     QList<OutSlot<AudioSoundSample>*>		getStaticAudioOutputList( void ) const;
    //     QList<InSlot<qreal>*>		getStaticControlInputList( void ) const;
    //     QList<OutSlot<qreal>*>		getStaticControlOutputList( void ) const;

//
// DYNAMIC SLOTS
//

//     QList<OutSlot<LightVideoFrame>*>    getDynamicVideoOutputList( void ) const;
//     QList<InSlot<LightVideoFrame>*>     getDynamicVideoInputList( void ) const;

//     QList<OutSlot<AudioSoundSample>*>    getDynamicAudioOutputList( void ) const;
//     QList<InSlot<AudioSoundSample>*>     getDynamicAudioInputList( void ) const;

//     QList<OutSlot<qreal>*>    getDynamicControlOutputList( void ) const;
//     QList<InSlot<qreal>*>     getDynamicControlInputList( void ) const;


//     QList<OutSlot<LightVideoFrame>*>    getDynamicVideoOutputById( quint32 id ) const;
//     QList<InSlot<LightVideoFrame>*>     getDynamicVideoInputById( quint32 id ) const;

//     QList<OutSlot<AudioSoundSample>*>    getDynamicAudioOutputById( quint32 id ) const;
//     QList<InSlot<AudioSoundSample>*>     getDynamicAudioInputById( quint32 id ) const;

//     QList<OutSlot<qreal>*>    getDynamicControlOutputById( quint32 ) const;
//     QList<InSlot<qreal>*>     getDynamicControlInputById( quint32 ) const;


    //
    //
    // GETTING EFFECT CONFIGURATION
    //
    //

    //
    // STATICS SLOTS
    //

    QList<QByteArray> const &	getStaticVideosInputsNameList( void ) const;
    QList<QByteArray> const &	getStaticVideosOutputsNameList( void ) const;
    //     QList<QByteArray> const &	getStaticAudiosInputsNameList( void ) const;
    //     QList<QByteArray> const &	getStaticAudiosOutputsNameList( void ) const;
    //     QList<QByteArray> const &	getStaticControlsInputsNameList( void ) const;
    //     QList<QByteArray> const &	getStaticControlsOutputsNameList( void ) const;

    QList<QByteArray> const &	getStaticVideosInputsIdList( void ) const;
    QList<QByteArray> const &	getStaticVideosOutputsIdList( void ) const;
    //     QList<QByteArray> const &	getStaticAudiosInputsIdList( void ) const;
    //     QList<QByteArray> const &	getStaticAudiosOutputsIdList( void ) const;
    //     QList<QByteArray> const &	getStaticControlsInputsIdList( void ) const;
    //     QList<QByteArray> const &	getStaticControlsOutputsIdList( void ) const;

    QByteArray const &          getStaticVideoInputNameById( quint32 const id ) const;
    QByteArray const &          getStaticVideoOutputNameById( quint32 const id ) const;
    //     QByteArray const &          getStaticAudioInputNameById( quint32 const id ) const;
    //     QByteArray const &          getStaticAudioOutputNameById( quint32 const id ) const;
    //     QByteArray const &          getStaticControlInputNameById( quint32 const id ) const;
    //     QByteArray const &          getStaticControlOutputNameById( quint32 const id ) const;

    quint32                     getStaticVideoInputIdByName( QByteArray const & name ) const;
    quint32                     getStaticVideoOutputIdByName( QByteArray const & name ) const;
    //     quint32                     getStaticAudioInputIdByName( QByteArray const & name ) const;
    //     quint32                     getStaticAudioOutputIdByName( QByteArray const & name ) const;
    //     quint32                     getStaticControlInputIdByName( QByteArray const & name ) const;
    //     quint32                     getStaticControlOutputIdByName( QByteArray const & name ) const;

    quint32                     getNBStaticVideosInputs( void ) const;
    quint32                     getNBStaticVideosOutputs( void ) const;
    //     quint32                     getNBStaticAudiosIntputs( void ) const;
    //     quint32                     getNBStaticAudiosOutputs( void ) const;
    //     quint32                     getNBStaticControlsInputs( void ) const;
    //     quint32                     getNBStaticControlsOutputs( void ) const;

    //
    // DYNAMICS SLOTS
    //

//     quint32             getNBDynamicVideosInputs( void ) const;
//     quint32             getNBDynamicVideosOutputs( void ) const;
//     quint32             getNBDynamicAudiosIntputs( void ) const;
//     quint32             getNBDynamicAudiosOutputs( void ) const;
//     quint32             getNBDynamicControlsInputs( void ) const;
//     quint32             getNBDynamicControlsOutputs( void ) const;

//     bool                areDynamicVideosInputsEnabled( void ) const;
//     bool                areDynamicVideosOutputsEnabled( void ) const;
//     bool                areDynamicAudiosIntputsEnabled( void ) const;
//     bool                areDynamicAudiosOutputsEnabled( void ) const;
//     bool                areDynamicControlsInputsEnabled( void ) const;
//     bool                areDynamicControlsOutputsEnabled( void ) const;

    //
    //
    //
    // GETTING EFFECT WIDGET
    //
    //
    //

//     QWidget*            getWidget( void );

    //
    //
    //
    // RENDER METHOD
    //
    //
    //

    void        render( void );

    //
    //
    //
    // CONNECTION AND BINDING METHODS
    //
    //
    //

    //
    //
    //
    // MISSING FUNCTIONNALITY
    //
    //
    //

    // IN PRIVATE : GETTING SLOT FOR THE EFFECT DEVELOPPER

    // TO FINISH : CONNECTION AND BINDINDG

    // INTERPRETER AND ITS PRIMITIVE METHODES

    // REPLACE VIRTUAL PURE RENDER METHOD BY VIRTUAL RENDER METHOD WHO RENTER THE SUB-EFFECTS BY DEFAULT
    // WITH THE MULTI-THREAD RENDER CLASS

    // EffectsFactory static instance into EffectNode

    // SUB-EFFECTS MANAGEMENT METHODS

    // BIEN SEPARER LE DEVELOPPER-LAND ET LE EFFECTSENGINE-LAND, notamment en faisant des interface IPublic*slot
    // DONC TOUT PROXIFIER PAR LE GENERICEFFECT, sauf l'ecriture/lecture dans un slot et le getting de l'id et de l'instance de l'effet et du EffectNode

    //
    //
    //
    // DEPRECATED
    //
    //
    //

    //
    // CONNECTIONS BETWEEN GENERICEFFECTS
    //

    /*   void				connectDynOutToStatIn( EffectNode* destinationeffect, QString const & inslotname ); */
    /*   void				connectDynOutToDynIn( EffectNode* destinationeffect ); */
    void                                connectOutput( quint32 outIndex, EffectNode* destEffect, quint32 inIndex );
    /*   void				connectStatOutToDynIn( QString const & outslotname, EffectNode* destinationeffect ); */

    //
    // CONNECTIONS BETWEEN GENERICEFFECT & OUTSLOTS/INSLOTS
    //

    void                                connect( OutSlot<LightVideoFrame> & out, quint32 inIndex );
    void                                connect( quint32 outIndex, InSlot<LightVideoFrame> & in);

    InSlot<LightVideoFrame> &           getVideoInput(quint32 id);
    OutSlot<LightVideoFrame> &          getVideoOutput(quint32 id);

    ////
    ////
    ////
    ////
    //// ATTRIBUTES
    ////
    ////
    ////
    ////


private:

    IEffectPlugin*                      m_plugin;
    InSlot<LightVideoFrame>*            m_videoInputs;
    OutSlot<LightVideoFrame>*           m_videoOutputs;

    //
    //
    //
    // EFFECT TYPE AND EFFECT INSTANCE INFORMATIONS
    //
    //
    //

    quint32                             m_typeId;
    QString                             m_typeName;
    quint32                             m_instanceId;
    QString                             m_instanceName;

    //
    //
    // SLOTS
    //
    //

    //
    // STATIC SLOTS
    //

    // VIDEOS SLOTS

    SemanticObjectManager< InSlot<LightVideoFrame> >	m_staticVideosInputs;
    SemanticObjectManager< OutSlot<LightVideoFrame> >	m_staticVideosOutputs;

    // AUDIOS SLOTS

    //     SemanticObjectManager< InSlot<AudioSoundSample> >  m_staticAudiosInputs;
    //     SemanticObjectManager< OutSlot<AudioSoundSample> > m_staticAudiosOutputs;

    // CONTROLS SLOTS

    //     SemanticObjectManager< InSlot<qreal> >	m_staticControlsInputs;
    //     SemanticObjectManager< OutSlot<qreal> >	m_staticControlsOutputs;

    //
    // DYNAMIC SLOTS
    //

    // VIDEOS SLOTS

    //     QList< InSlot<LightVideoFrame>* >                    m_dynamicVideosInputs;
    //     QList< OutSlot<LightVideoFrame>* >                   m_dynamicVideosOutputs;

    // AUDIOS SLOTS

    // QList< InSlot<AudioSoundSample>* >                                m_dynamicAudiosInputs;
    // QList< OutSlot<AudioSoundSample>* >                               m_dynamicAudiosOutputs;

    // CONTROLS SLOTS

    //     QList< InSlot<qreal>* >                              m_dynamicControlsInputs;
    //     QList< OutSlot<qreal>* >                             m_dynamicControlsOutputs;

    //
    // DYNAMICS SLOTS INFOS
    //

    // VIDEOS SLOTS

    //     bool                                        m_enableDynamicVideosInputs;
    //     bool                                        m_enableDynamicVideosOutputs;

    // AUDIOS SLOTS

    //     bool                                        m_enableDynamicAudiosInputs;
    //     bool                                        m_enableDynamicAudiossOutputs;

    // CONTROLS SLOTS

    //     bool                                        m_enableDynamicControlInputs;
    //     bool                                        m_enableDynamicControlOutputs;
};

#endif // EFFECTNODE_H_
