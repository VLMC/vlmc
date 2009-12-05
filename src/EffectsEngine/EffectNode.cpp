/*****************************************************************************
 * EffectNode.cpp: Node of the patch
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

#include "EffectNode.h"

template class SemanticObjectManager< InSlot<LightVideoFrame> >;
template class SemanticObjectManager< OutSlot<LightVideoFrame> >;

// template class SemanticObjectManager<InSlot<AudioSoundSample> >;
// template class SemanticObjectManager<OutSlot<AudioSoundSample> >;

// template class SemanticObjectManager<InSlot<qreal> >;
// template class SemanticObjectManager<OutSlot<qreal> >;

EffectNode::EffectNode(IEffectPlugin* plugin) : m_father( NULL ), m_plugin( plugin ), m_videoInputs( NULL ), m_videoOutputs( NULL )
{
    m_plugin->init(this);
    m_staticVideosInputs.setFather( this );
    m_staticVideosOutputs.setFather( this );
}

EffectNode::~EffectNode()
{
    delete m_plugin;
    if ( m_videoInputs )
        delete [] m_videoInputs;
    if ( m_videoOutputs )
        delete [] m_videoOutputs;
}


//-------------------------------------------------------------------
//
// DEPRECATED
//
//

void                                    EffectNode::init( quint32 const nbvideoinputs,
                                                             quint32 const nbvideooutputs )
{
    m_videoInputs = new InSlot<LightVideoFrame>[nbvideoinputs];
    m_videoOutputs = new OutSlot<LightVideoFrame>[nbvideooutputs];
    return ;
}

void                                    EffectNode::render( void )
{
    m_plugin->render();
    return ;
}

//
//
//
// CONNECTION AND BINDING METHODS
//
//
//

//
//
// VIDEO SLOTS
//
//

//
// STATIC TO STATIC
//

// bool        EffectNode::connectStaticVideoOutputToStaticVideoInput( QString const & outName, QString const & nodeName, QString const & inName )
// {
// }

// bool        EffectNode::connectStaticVideoOutputToStaticVideoInput( QString const & outName, QString const & nodeName, quint32 inId )
// {
// }

// bool        EffectNode::connectStaticVideoOutputToStaticVideoInput( QString const & outName, quint32 nodeId, QString const & inName )
// {
// }

// bool        EffectNode::connectStaticVideoOutputToStaticVideoInput( QString const & outName, quint32 nodeId, quint32 inId )
// {
// }

// bool        EffectNode::connectStaticVideoOutputToStaticVideoInput( quint32 outId, QString const & nodeName, QString const & inName )
// {
// }

// bool        EffectNode::connectStaticVideoOutputToStaticVideoInput( quint32 outId, QString const & nodeName, quint32 inId )
// {
// }

// bool        EffectNode::connectStaticVideoOutputToStaticVideoInput( quint32 outId, quint32 nodeId, QString const & inName )
// {
// }

// bool        EffectNode::connectStaticVideoOutputToStaticVideoInput( quint32 outId, quint32 nodeId, quint32 inId )
// {
// }

// bool        EffectNode::connectStaticVideoOutputToDynamicVideoInput( QString const & outName, quint32 nodeId )
// {
// }

// bool        EffectNode::connectStaticVideoOutputToDynamicVideoInput( QString const & outName, QString const & nodeName )
// {
// }

// bool        EffectNode::connectStaticVideoOutputToDynamicVideoInput( quint32 outId, QString const & nodeName )
// {
// }

// bool        EffectNode::connectStaticVideoOutputToDynamicVideoInput( quint32 outId, quint32 nodeId )
// {
// }

// bool        EffectNode::connectDynamicVideoOutputToStaticVideoInput( QString const & nodeName, QString const & inName )
// {
// }

// bool        EffectNode::connectDynamicVideoOutputToStaticVideoInput( QString const & nodeName, quint32 inId )
// {
// }

// bool        EffectNode::connectDynamicVideoOutputToStaticVideoInput( quint32 nodeId, QString const & inName )
// {
// }

// bool        EffectNode::connectDynamicVideoOutputToStaticVideoInput( quint32 nodeId, quint32 inId )
// {
// }

// bool        EffectNode::connectDynamicVideoOutputToDynamicVideoInput( QString const & nodeName )
// {
// }

// bool        EffectNode::connectDynamicVideoOutputToDynamicVideoInput( quint32 nodeId )
// {
// }

// bool        EffectNode::disconnectDynamicVideoOutput( void )
// {
// }

// bool        EffectNode::disconnectStaticVideoOutput( quint32 nodeId )
// {
// }

// bool        EffectNode::disconnectStaticVideoOutput( QString const & nodeName )
// {
// }

//
//
//
// GET INTERNAL PLUGIN
//
//
//

IEffectPlugin*                          EffectNode::getInternalPlugin( void )
{
    return ( m_plugin );
}


//
//
//
// EFFECT INSTANCE AND EFFECT TYPE INFORMATIONS
//
//
//

void                    EffectNode::setFather( EffectNode* father )
{
    m_father = father;
    return ;
}

IEffectNode*            EffectNode::getFather( void ) const
{
    return ( m_father );
}

EffectNode*             EffectNode::getPrivateFather( void ) const
{
    return ( m_father );
}



void                    EffectNode::setTypeId( quint32 typeId )
{
    m_typeId = typeId;
    return ;
}

void                    EffectNode::setTypeName( QString const & typeName )
{
    m_typeName = typeName;
    return ;
}

void                    EffectNode::setInstanceId( quint32 instanceId )
{
    m_instanceId = instanceId;
    return ;
}

void                    EffectNode::setInstanceName( QString const & instanceName )
{
    m_instanceName = instanceName;
    return ;
}

quint32                 EffectNode::getTypeId( void ) const
{
    return ( m_typeId );
}

QString const &         EffectNode::getTypeName( void ) const
{
    return ( m_typeName );
}

quint32                 EffectNode::getInstanceId( void ) const
{
    return ( m_instanceId );
}

QString const &         EffectNode::getInstanceName( void ) const
{
    return ( m_instanceName );
}

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

void		EffectNode::createStaticVideoInput( QString const & name )
{
    m_staticVideosInputs.createObject( name );
    return ;
}

void		EffectNode::createStaticVideoOutput( QString const & name )
{
    m_staticVideosOutputs.createObject( name );
    return ;
}

//     void		addStaticAudioInput( QByteArray const & name );
//     void		addStaticAudioOutput( QByteArray const & name );
//     void		addStaticControlInput( QByteArray const & name );
//     void		addStaticControlOutput( QByteArray const & name );

void		EffectNode::createStaticVideoInput( void )
{
    m_staticVideosInputs.createObject();
    return ;
}

void		EffectNode::createStaticVideoOutput( void )
{
    m_staticVideosOutputs.createObject();
    return ;
}
//     void		addStaticAudioInput( void );
//     void		addStaticAudioOutput( void );
//     void		addStaticControlInput( void );
//     void		addStaticControlOutput( void );


bool		EffectNode::removeStaticVideoInput( QString const & name )
{
    return ( m_staticVideosInputs.deleteObject( name ) );
}

bool		EffectNode::removeStaticVideoOutput( QString const & name )
{
    return ( m_staticVideosOutputs.deleteObject( name ) );
}
//     bool		removeStaticAudioInput( QByteArray const & name );
//     bool		removeStaticAudioOutput( QByteArray const & name );
//     bool		removeStaticControlInput( QByteArray const & name );
//     bool		removeStaticControlOutput( QByteArray const & name );

bool		EffectNode::removeStaticVideoInput( quint32 id )
{
    return ( m_staticVideosInputs.deleteObject( id ) );
}

bool		EffectNode::removeStaticVideoOutput( quint32 id )
{
    return ( m_staticVideosOutputs.deleteObject( id ) );
}

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

InSlot<LightVideoFrame>*		EffectNode::getStaticVideoInput( QString const & name ) const
{
    return ( m_staticVideosInputs.getObject( name ) );
}

OutSlot<LightVideoFrame>*	        EffectNode::getStaticVideoOutput( QString const & name ) const
{
    return ( m_staticVideosOutputs.getObject( name ) );
}

//     InSlot<AudioSoundSample>*		getStaticAudioInput( QByteArray const & name ) const;
//     OutSlot<AudioSoundSample>*		getStaticAudioOutput( QByteArray const & name ) const;
//     InSlot<qreal>*		getStaticControlInput( QByteArray const & name ) const;
//     OutSlot<qreal>* 		getStaticControlOutput( QByteArray const & name ) const;

InSlot<LightVideoFrame>*		EffectNode::getStaticVideoInput( quint32 id ) const
{
    return ( m_staticVideosInputs.getObject( id ) );
}

OutSlot<LightVideoFrame>*		EffectNode::getStaticVideoOutput( quint32 id ) const
{
    return ( m_staticVideosOutputs.getObject( id ) );
}

//  InSlot<AudioSoundSample>*		getStaticAudioInput( quint32 id ) const;
//  OutSlot<AudioSoundSample>*		getStaticAudioOutput( quint32 id ) const;
//  InSlot<qreal>*		getStaticControlInput( quint32 id ) const;
//  OutSlot<qreal>*		getStaticControlOutput( quint32 id ) const;

QList<InSlot<LightVideoFrame>*>	EffectNode::getStaticsVideosInputsList( void ) const
{
    return ( m_staticVideosInputs.getObjectsList() );
}

QList<OutSlot<LightVideoFrame>*>		EffectNode::getStaticsVideosOutputsList( void ) const
{
    return ( m_staticVideosOutputs.getObjectsList() );
}

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

QList<QString>	EffectNode::getStaticsVideosInputsNamesList( void ) const
{
    return ( m_staticVideosInputs.getObjectsNamesList() );
}

QList<QString>	EffectNode::getStaticsVideosOutputsNamesList( void ) const
{
    return ( m_staticVideosOutputs.getObjectsNamesList() );
}

//     QList<QByteArray> const &	getStaticAudiosInputsNameList( void ) const;
//     QList<QByteArray> const &	getStaticAudiosOutputsNameList( void ) const;
//     QList<QByteArray> const &	getStaticControlsInputsNameList( void ) const;
//     QList<QByteArray> const &	getStaticControlsOutputsNameList( void ) const;

QList<quint32>	EffectNode::getStaticsVideosInputsIdsList( void ) const
{
    return ( m_staticVideosInputs.getObjectsIdsList() );
}

QList<quint32>	EffectNode::getStaticsVideosOutputsIdsList( void ) const
{
    return ( m_staticVideosOutputs.getObjectsIdsList() );
}

//     QList<QByteArray> const &	getStaticAudiosInputsIdList( void ) const;
//     QList<QByteArray> const &	getStaticAudiosOutputsIdList( void ) const;
//     QList<QByteArray> const &	getStaticControlsInputsIdList( void ) const;
//     QList<QByteArray> const &	getStaticControlsOutputsIdList( void ) const;

QString const           EffectNode::getStaticVideoInputNameById( quint32 const id ) const
{
    return ( m_staticVideosInputs.getObjectNameByObjectId( id ) );
}

QString const           EffectNode::getStaticVideoOutputNameById( quint32 const id ) const
{
    return ( m_staticVideosOutputs.getObjectNameByObjectId( id ) );
}

//     QByteArray const &          getStaticAudioInputNameById( quint32 const id ) const;
//     QByteArray const &          getStaticAudioOutputNameById( quint32 const id ) const;
//     QByteArray const &          getStaticControlInputNameById( quint32 const id ) const;
//     QByteArray const &          getStaticControlOutputNameById( quint32 const id ) const;

quint32                     EffectNode::getStaticVideoInputIdByName( QString const & name ) const
{
    return ( m_staticVideosInputs.getObjectIdByObjectName( name ) );
}

quint32                     EffectNode::getStaticVideoOutputIdByName( QString const & name ) const
{
    return ( m_staticVideosOutputs.getObjectIdByObjectName( name ) );
}

//     quint32                     getStaticAudioInputIdByName( QByteArray const & name ) const;
//     quint32                     getStaticAudioOutputIdByName( QByteArray const & name ) const;
//     quint32                     getStaticControlInputIdByName( QByteArray const & name ) const;
//     quint32                     getStaticControlOutputIdByName( QByteArray const & name ) const;

quint32                     EffectNode::getNBStaticsVideosInputs( void ) const
{
    return ( m_staticVideosInputs.getNBObjects() );
}

quint32                     EffectNode::getNBStaticsVideosOutputs( void ) const
{
    return ( m_staticVideosOutputs.getNBObjects() );
}

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

InSlot<LightVideoFrame> &               EffectNode::getVideoInput(quint32 id)
{
    return ( m_videoInputs[id] );
}

OutSlot<LightVideoFrame> &              EffectNode::getVideoOutput(quint32 id)
{
    return ( m_videoOutputs[id] );
}


//-------------------------------------------------------------------
//
// DEPRECATED
//
//

// CONNECTIONS BETWEEN GENERICEFFECTS

void				EffectNode::connectOutput( quint32 outIndex, EffectNode* destEffect, quint32 inIndex)
{
  // THINK TO CHECK IF THE SLOTS EXISTS BY CALLING THE PRIVATES METHODS!!!
  ( m_videoOutputs[outIndex] ).connect( destEffect->m_videoInputs[inIndex] );
  return ;
}

// CONNECTIONS DETWEEN GENERICEFFECT & OUTSLOT/INSLOT

void				EffectNode::connect( OutSlot<LightVideoFrame> & out, quint32 inIndex )
{
  out.connect( m_videoInputs[inIndex] );
  return ;
}

void				EffectNode::connect( quint32 outIndex, InSlot<LightVideoFrame> & in )
{
  ( m_videoOutputs[outIndex] ).connect( in );
  return ;
}
