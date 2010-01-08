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

#include "IEffectNode.h"
#include "IEffectPlugin.h"

#include <QObject>
#include <QReadLocker>
#include <QReadWriteLock>
#include <QString>
#include <QWriteLocker>

EffectNodeFactory              EffectNode::s_renf;
QReadWriteLock                 EffectNode::s_srwl( QReadWriteLock::Recursive );

template class SemanticObjectManager< InSlot<LightVideoFrame> >;
template class SemanticObjectManager< OutSlot<LightVideoFrame> >;

// template class SemanticObjectManager<InSlot<AudioSoundSample> >;
// template class SemanticObjectManager<OutSlot<AudioSoundSample> >;

// template class SemanticObjectManager<InSlot<qreal> >;
// template class SemanticObjectManager<OutSlot<qreal> >;

EffectNode::EffectNode( IEffectPlugin* plugin ) : m_rwl( QReadWriteLock::Recursive ),
                                                  m_father( NULL ), m_plugin( plugin ),
                                                  m_visited( false )
{
    m_staticVideosInputs.setFather( this );
    m_staticVideosOutputs.setFather( this );
    m_staticVideosInputs.setScope( false );
    m_staticVideosOutputs.setScope( false );
    m_internalsStaticVideosInputs.setFather( this );
    m_internalsStaticVideosOutputs.setFather( this );
    m_internalsStaticVideosInputs.setScope( true );
    m_internalsStaticVideosOutputs.setScope( true );
    m_enf.setFather( this );
    m_plugin->init( this );
}


EffectNode::EffectNode() : m_father( NULL ),
                           m_plugin( NULL ),
                           m_visited( false )
{
    m_staticVideosInputs.setFather( this );
    m_staticVideosOutputs.setFather( this );
    m_staticVideosInputs.setScope( false );
    m_staticVideosOutputs.setScope( false );
    m_internalsStaticVideosInputs.setFather( this );
    m_internalsStaticVideosOutputs.setFather( this );
    m_internalsStaticVideosInputs.setScope( true );
    m_internalsStaticVideosOutputs.setScope( true );
    m_enf.setFather( this );
}

EffectNode::~EffectNode()
{
    delete m_plugin;
}

void
EffectNode::render( void )
{
    if ( m_plugin != NULL )
        m_plugin->render();
    else
    {
        if ( m_father != NULL)
        {
            QWriteLocker                        wl( &m_rwl );
            transmitDatasFromInputsToInternalsOutputs();
            renderSubNodes();
            transmitDatasFromInternalsInputsToOutputs();
            resetAllChildsNodesVisitState();
        }
        else
        {
            QWriteLocker                        wl( &m_rwl );
            renderSubNodes();
            resetAllChildsNodesVisitState();
        }
    }
}

void
EffectNode::renderSubNodes( void )
{
    QList<EffectNode*>                                effectsNodes = m_enf.getEffectNodeInstancesList();
    QList<EffectNode*>::iterator                      effectsNodesIt = effectsNodes.begin();
    QList<EffectNode*>::iterator                      effectsNodesEnd = effectsNodes.end();
    QList<OutSlot<LightVideoFrame>*>                  intOuts = m_connectedInternalsStaticVideosOutputs.getObjectsReferencesList() ;
    QList<OutSlot<LightVideoFrame>*>::iterator        intOutsIt = intOuts.begin();
    QList<OutSlot<LightVideoFrame>*>::iterator        intOutsEnd = intOuts.end();
    QQueue<EffectNode*>                               nodeQueue;
    EffectNode*                                       toQueueNode;
    EffectNode*                                       currentNode;
    InSlot<LightVideoFrame>*                          currentIn;

    for ( ; effectsNodesIt != effectsNodesEnd; ++effectsNodesIt )
    {
        if (
            ( (*effectsNodesIt)->getNBConnectedStaticsVideosInputs() == 0 ) &&
            ( (*effectsNodesIt)->getNBConnectedStaticsVideosOutputs() > 0 )
            )
        {
            (*effectsNodesIt)->setVisited();
            nodeQueue.enqueue( (*effectsNodesIt) );
        }
    }
    for ( ; intOutsIt != intOutsEnd; ++intOutsIt )
    {
        currentIn = (*intOutsIt)->getInSlotPtr();
        toQueueNode = currentIn->getPrivateFather();
        if ((toQueueNode != this) && ( toQueueNode->wasItVisited() == false ))
        {
            toQueueNode->setVisited();
            nodeQueue.enqueue( toQueueNode );
        }
    }

    while ( nodeQueue.empty() == false )
    {
        currentNode = nodeQueue.dequeue();
        QList<OutSlot<LightVideoFrame>*>                  outs = currentNode->getConnectedStaticsVideosOutputsList();
        QList<OutSlot<LightVideoFrame>*>::iterator        outsIt = outs.begin();
        QList<OutSlot<LightVideoFrame>*>::iterator        outsEnd = outs.end();

        currentNode->render();
        for ( ; outsIt != outsEnd; ++outsIt )
        {
            currentIn = (*outsIt)->getInSlotPtr();
            toQueueNode = currentIn->getPrivateFather();
            if ((toQueueNode != this) && ( toQueueNode->wasItVisited() == false ))
            {
                toQueueNode->setVisited();
                nodeQueue.enqueue( toQueueNode );
            }
        }
    }
}

void
EffectNode::transmitDatasFromInputsToInternalsOutputs( void )
{
    if ( m_staticVideosInputs.getNBObjects() != 0 )
    {
        QList<InSlot<LightVideoFrame>*>             ins = m_staticVideosInputs.getObjectsList();
        QList<OutSlot<LightVideoFrame>*>            intOuts = m_internalsStaticVideosOutputs.getObjectsList();
        QList<InSlot<LightVideoFrame>*>::iterator  insIt = ins.begin();
        QList<InSlot<LightVideoFrame>*>::iterator  insEnd = ins.end();
        QList<OutSlot<LightVideoFrame>*>::iterator  intOutsIt = intOuts.begin();
        QList<OutSlot<LightVideoFrame>*>::iterator  intOutsEnd = intOuts.end();

        for ( ; ( insIt != insEnd ) && ( intOutsIt != intOutsEnd ); ++insIt, ++intOutsIt )
            *(*intOutsIt) << *(*insIt);
    }

}

void
EffectNode::transmitDatasFromInternalsInputsToOutputs( void )
{
    if ( m_staticVideosOutputs.getNBObjects() != 0 )
    {
        QList<InSlot<LightVideoFrame>*>             intIns = m_internalsStaticVideosInputs.getObjectsList();
        QList<OutSlot<LightVideoFrame>*>            outs = m_staticVideosOutputs.getObjectsList();
        QList<InSlot<LightVideoFrame>*>::iterator  intInsIt = intIns.begin();
        QList<InSlot<LightVideoFrame>*>::iterator  intInsEnd = intIns.end();
        QList<OutSlot<LightVideoFrame>*>::iterator  outsIt = outs.begin();
        QList<OutSlot<LightVideoFrame>*>::iterator  outsEnd = outs.end();

        for ( ; ( intInsIt != intInsEnd ) && ( outsIt != outsEnd ); ++intInsIt, ++outsIt )
            *(*outsIt) << *(*intInsIt);
    }
}

void
EffectNode::resetAllChildsNodesVisitState( void )
{
    QList<EffectNode*>            childs = m_enf.getEffectNodeInstancesList();

    if ( childs.empty() == false )
    {
        QList<EffectNode*>::iterator  it = childs.begin();
        QList<EffectNode*>::iterator  end = childs.end();

        for ( ; it != end; ++it)
            (*it)->resetVisitState();
    }
}

void
EffectNode::setVisited( void )
{
    QWriteLocker wl( &m_rwl );
    m_visited = true;
}

void
EffectNode::resetVisitState( void )
{
    QWriteLocker wl( &m_rwl );
    m_visited = false;
}

bool
EffectNode::wasItVisited( void ) const
{
    QReadLocker rl( &m_rwl );
    return  m_visited;
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

//------------------------------------------------------------//
//                      CONNECT/DISCONNECT                    //
//------------------------------------------------------------//

// ----------------  CONNECT STATIC TO STATIC -------------------

bool
EffectNode::connectStaticVideoOutputToStaticVideoInput( const QString & outName, const QString & nodeName, const QString & inName )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    EffectNode*                brother;
    InSlot<LightVideoFrame>*   in;

    if ( ( out = m_staticVideosOutputs.getObject( outName ) ) == NULL )
        return false;
    if ( m_father == NULL )
        return false;
    if ( ( brother = m_father->getChild( nodeName ) ) == NULL )
        return false;
    if ( brother == this )
    {
        if ( ( in = m_staticVideosInputs.getObject( inName ) ) == NULL )
            return false;
        if ( out->connect( *in ) == false )
            return false;
        if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
            return false;
        if ( m_connectedStaticVideosInputs.addObjectReference( in ) == false )
            return false;
    }
    else
    {
        if ( ( in = brother->getStaticVideoInput( inName ) ) == NULL )
            return false;
        if ( out->connect( *in ) == false )
            return false;
        if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
            return false;
        if ( brother->referenceStaticVideoInputAsConnected( in ) == false )
            return false;
    }
    return true;
}

bool
EffectNode::connectStaticVideoOutputToStaticVideoInput( const QString & outName, const QString & nodeName, quint32 inId )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    EffectNode*                brother;
    InSlot<LightVideoFrame>*  in;

    if ( ( out = m_staticVideosOutputs.getObject( outName ) ) == NULL )
        return false;
    if ( m_father == NULL )
        return false;
    if ( ( brother = m_father->getChild( nodeName ) ) == NULL )
        return false;
    if ( brother == this )
    {
        if ( ( in = m_staticVideosInputs.getObject( inId ) ) == NULL )
            return false;
        if ( out->connect( *in ) == false )
            return false;
        if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
            return false;
        if ( m_connectedStaticVideosInputs.addObjectReference( in ) == false )
            return false;
    }
    else
    {
        if ( ( in = brother->getStaticVideoInput( inId ) ) == NULL )
            return false;
        if ( out->connect( *in ) == false )
            return false;
        if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
            return false;
        if ( brother->referenceStaticVideoInputAsConnected( in ) == false )
            return false;
    }
    return true;
}

bool
EffectNode::connectStaticVideoOutputToStaticVideoInput( const QString & outName, quint32 nodeId, const QString & inName )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    EffectNode*                brother;
    InSlot<LightVideoFrame>*  in;

    if ( ( out = m_staticVideosOutputs.getObject( outName ) ) == NULL )
        return false;
    if ( m_father == NULL )
        return false;
    if ( ( brother = m_father->getChild( nodeId ) ) == NULL )
        return false;
    if ( brother == this )
    {
        if ( ( in = m_staticVideosInputs.getObject( inName ) ) == NULL )
            return false;
        if ( out->connect( *in ) == false )
            return false;
        if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
            return false;
        if ( m_connectedStaticVideosInputs.addObjectReference( in ) == false )
            return false;
    }
    else
    {
        if ( ( in = brother->getStaticVideoInput( inName ) ) == NULL )
            return false;
        if ( out->connect( *in ) == false )
            return false;
        if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
            return false;
        if ( brother->referenceStaticVideoInputAsConnected( in ) == false )
            return false;
    }
    return true;
}

bool
EffectNode::connectStaticVideoOutputToStaticVideoInput( const QString & outName, quint32 nodeId, quint32 inId )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    EffectNode*                brother;
    InSlot<LightVideoFrame>*  in;

    if ( ( out = m_staticVideosOutputs.getObject( outName ) ) == NULL )
        return false;
    if ( m_father == NULL )
        return false;
    if ( ( brother = m_father->getChild( nodeId ) ) == NULL )
        return false;
    if ( brother == this )
    {
        if ( ( in = m_staticVideosInputs.getObject( inId ) ) == NULL )
            return false;
        if ( out->connect( *in ) == false )
            return false;
        if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
            return false;
        if ( m_connectedStaticVideosInputs.addObjectReference( in ) == false )
            return false;
    }
    else
    {
        if ( ( in = brother->getStaticVideoInput( inId ) ) == NULL )
            return false;
        if ( out->connect( *in ) == false )
            return false;
        if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
            return false;
        if ( brother->referenceStaticVideoInputAsConnected( in ) == false )
            return false;
    }
    return true;
}

bool
EffectNode::connectStaticVideoOutputToStaticVideoInput( quint32 outId, const QString & nodeName, const QString & inName )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    EffectNode*                brother;
    InSlot<LightVideoFrame>*  in;

    if ( ( out = m_staticVideosOutputs.getObject( outId ) ) == NULL )
        return false;
    if ( m_father == NULL )
        return false;
    if ( ( brother = m_father->getChild( nodeName ) ) == NULL )
        return false;
    if ( brother == this )
    {
        if ( ( in = m_staticVideosInputs.getObject( inName ) ) == NULL )
            return false;
        if ( out->connect( *in ) == false )
            return false;
        if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
            return false;
        if ( m_connectedStaticVideosInputs.addObjectReference( in ) == false )
            return false;
    }
    else
    {
        if ( ( in = brother->getStaticVideoInput( inName ) ) == NULL )
            return false;
        if ( out->connect( *in ) == false )
            return false;
        if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
            return false;
        if ( brother->referenceStaticVideoInputAsConnected( in ) == false )
            return false;
    }
    return true;
}

bool
EffectNode::connectStaticVideoOutputToStaticVideoInput( quint32 outId, const QString & nodeName, quint32 inId )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    EffectNode*                brother;
    InSlot<LightVideoFrame>*  in;

    if ( ( out = m_staticVideosOutputs.getObject( outId ) ) == NULL )
        return false;
    if ( m_father == NULL )
        return false;
    if ( ( brother = m_father->getChild( nodeName ) ) == NULL )
        return false;
    if ( brother == this )
    {
        if ( ( in = m_staticVideosInputs.getObject( inId ) ) == NULL )
            return false;
        if ( out->connect( *in ) == false )
            return false;
        if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
            return false;
        if ( m_connectedStaticVideosInputs.addObjectReference( in ) == false )
            return false;
    }
    else
    {
        if ( ( in = brother->getStaticVideoInput( inId ) ) == NULL )
            return false;
        if ( out->connect( *in ) == false )
            return false;
        if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
            return false;
        if ( brother->referenceStaticVideoInputAsConnected( in ) == false )
            return false;
    }
    return true;
}

bool
EffectNode::connectStaticVideoOutputToStaticVideoInput( quint32 outId, quint32 nodeId, const QString & inName )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    EffectNode*                brother;
    InSlot<LightVideoFrame>*  in;

    if ( ( out = m_staticVideosOutputs.getObject( outId ) ) == NULL )
        return false;
    if ( m_father == NULL )
        return false;
    if ( ( brother = m_father->getChild( nodeId ) ) == NULL )
        return false;
    if ( brother == this )
    {
        if ( ( in = m_staticVideosInputs.getObject( inName ) ) == NULL )
            return false;
        if ( out->connect( *in ) == false )
            return false;
        if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
            return false;
        if ( m_connectedStaticVideosInputs.addObjectReference( in ) == false )
            return false;
    }
    else
    {
        if ( ( in = brother->getStaticVideoInput( inName ) ) == NULL )
            return false;
        if ( out->connect( *in ) == false )
            return false;
        if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
            return false;
        if ( brother->referenceStaticVideoInputAsConnected( in ) == false )
            return false;
    }
    return true;
}

bool
EffectNode::connectStaticVideoOutputToStaticVideoInput( quint32 outId, quint32 nodeId, quint32 inId )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    EffectNode*                brother;
    InSlot<LightVideoFrame>*  in;

    if ( ( out = m_staticVideosOutputs.getObject( outId ) ) == NULL )
        return false;
    if ( m_father == NULL )
        return false;
    if ( ( brother = m_father->getChild( nodeId ) ) == NULL )
        return false;
    if ( brother == this )
    {
        if ( ( in = m_staticVideosInputs.getObject( inId ) ) == NULL )
            return false;
        if ( out->connect( *in ) == false )
            return false;
        if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
            return false;
        if ( m_connectedStaticVideosInputs.addObjectReference( in ) == false )
            return false;
    }
    else
    {
        if ( ( in = brother->getStaticVideoInput( inId ) ) == NULL )
            return false;
        if ( out->connect( *in ) == false )
            return false;
        if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
            return false;
        if ( brother->referenceStaticVideoInputAsConnected( in ) == false )
            return false;
    }
    return true;
}

// ----------------  CONNECT STATIC TO DYNAMIC -------------------

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

// ----------------  CONNECT DYNAMIC TO STATIC -------------------

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

// ----------------  CONNECT DYNAMIC TO DYNAMIC -------------------

// bool        EffectNode::connectDynamicVideoOutputToDynamicVideoInput( QString const & nodeName )
// {
// }

// bool        EffectNode::connectDynamicVideoOutputToDynamicVideoInput( quint32 nodeId )
// {
// }

// ----------------  DISCONNECT -------------------

// bool        EffectNode::disconnectDynamicVideoOutput( void )
// {
// }

bool
EffectNode::disconnectStaticVideoOutput( quint32 nodeId )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    InSlot<LightVideoFrame>*   in;
    EffectNode*                father;

    if ( ( out = m_staticVideosOutputs.getObject( nodeId ) ) == NULL )
        return false;

    in = out->getInSlotPtr();
    if ( out->disconnect() == false )
        return false;
    if ( m_connectedStaticVideosOutputs.delObjectReference( out->getId() ) == false )
        return false;
    father = in->getPrivateFather();
    if ( father == m_father )
    {
        if ( father->dereferenceInternalStaticVideoInputAsConnected( in->getId() ) == false )
            return false;
    }
    else if ( father == this )
    {
        if ( m_connectedStaticVideosInputs.delObjectReference( in->getId() ) == false )
            return false;
    }
    else
    {
        if ( father->dereferenceStaticVideoInputAsConnected( in->getId() ) == false )
            return false;
    }
    return true;
}

bool
EffectNode::disconnectStaticVideoOutput( const QString & nodeName )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    InSlot<LightVideoFrame>*   in;
    EffectNode*                father;

    if ( ( out = m_staticVideosOutputs.getObject( nodeName ) ) == NULL )
        return false;
    in = out->getInSlotPtr();
    if ( out->disconnect() == false )
        return false;
    if ( m_connectedStaticVideosOutputs.delObjectReference( out->getId() ) == false )
        return false;
    father = in->getPrivateFather();
    if ( father == m_father )
    {
        if ( father->dereferenceInternalStaticVideoInputAsConnected( in->getId() ) == false )
            return false;
    }
    else if ( father == this )
    {
        if ( m_connectedStaticVideosInputs.delObjectReference( in->getId() ) == false)
            return false;
    }
    else
    {
        if ( father->dereferenceStaticVideoInputAsConnected( in->getId() ) == false )
            return false;
    }
    return true;
}

//
//
//
// GET INTERNAL PLUGIN
//
//
//

IEffectPlugin*
EffectNode::getInternalPlugin( void )
{
    QReadLocker                        rl( &m_rwl );
    return m_plugin;
}


//
//
//
// EFFECT INSTANCE AND EFFECT TYPE INFORMATIONS
//
//
//

void
EffectNode::setFather( EffectNode* father )
{
    QWriteLocker                        wl( &m_rwl );
    m_father = father;

}

IEffectNode*
EffectNode::getFather( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_father;
}

EffectNode*
EffectNode::getPrivateFather( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_father;
}



void
EffectNode::setTypeId( quint32 typeId )
{
    QWriteLocker                        wl( &m_rwl );
    m_typeId = typeId;

}

void
EffectNode::setTypeName( const QString & typeName )
{
    QWriteLocker                        wl( &m_rwl );
    m_typeName = typeName;

}

void
EffectNode::setInstanceId( quint32 instanceId )
{
    QWriteLocker                        wl( &m_rwl );
    m_instanceId = instanceId;

}

void
EffectNode::setInstanceName( const QString & instanceName )
{
    QWriteLocker                        wl( &m_rwl );
    m_instanceName = instanceName;

}

quint32
EffectNode::getTypeId( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_typeId;
}

const QString &
EffectNode::getTypeName( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_typeName;
}

quint32
EffectNode::getInstanceId( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_instanceId;
}

const QString &
EffectNode::getInstanceName( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_instanceName;
}

bool
EffectNode::isAnEmptyNode( void ) const
{
    QReadLocker                        rl( &m_rwl );
    if ( m_plugin )
        return false;
    return true;
}

// ================================================================= ROOT NODES ========================================================================

bool
EffectNode::createRootNode( const QString & rootNodeName )
{
    QWriteLocker                        wl( &s_srwl );
    return EffectNode::s_renf.createEmptyEffectNodeInstance( rootNodeName );
}

bool
EffectNode::deleteRootNode( const QString & rootNodeName )
{
    QWriteLocker                        wl( &s_srwl );
    return EffectNode::s_renf.deleteEffectNodeInstance( rootNodeName );
}

EffectNode*
EffectNode::getRootNode( const QString & rootNodeName )
{
    QReadLocker                        rl( &s_srwl );
    return EffectNode::s_renf.getEffectNodeInstance( rootNodeName );
}

//
//
//
// PENSER UN JOUR A FAIRE LE RESTE DES WRAPPER , PAR ID, LIST, TRADUCTION, ETC.
//
//
//


// ================================================================= CHILD NODES ========================================================================

// ------------------- CHILDS TYPES INFORMATIONS -------------------

QList<QString>
EffectNode::getChildsTypesNamesList( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_enf.getEffectNodeTypesNamesList();
}

QList<quint32>
EffectNode::getChildsTypesIdsList( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_enf.getEffectNodeTypesIdsList();
}

const QString
EffectNode::getChildTypeNameByTypeId( quint32 typeId ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_enf.getEffectNodeTypeNameByTypeId( typeId );
}

quint32
EffectNode::getChildTypeIdByTypeName( const QString & typeName ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_enf.getEffectNodeTypeIdByTypeName( typeName );
}

// ------------------- CHILDS INFORMATIONS -------------------

QList<QString>
EffectNode::getChildsNamesList( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_enf.getEffectNodeInstancesNamesList();
}

QList<quint32>
EffectNode::getChildsIdsList( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_enf.getEffectNodeInstancesIdsList();
}

const QString
EffectNode::getChildNameByChildId( quint32 childId ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_enf.getEffectNodeInstanceNameByInstanceId( childId );
}

quint32
EffectNode::getChildIdByChildName( const QString & childName ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_enf.getEffectNodeInstanceIdByInstanceName( childName );
}

// ------------------- CREATE AND DELETE CHILDS -------------------

bool
EffectNode::createEmptyChild( void )
{
    QWriteLocker                        wl( &m_rwl );
    if ( m_plugin == NULL )
    {
        m_enf.createEmptyEffectNodeInstance();
        return true;
    }
    return false;
}

bool
EffectNode::createEmptyChild( const QString & childName )
{
    QWriteLocker                        wl( &m_rwl );
    if ( m_plugin == NULL )
        return m_enf.createEmptyEffectNodeInstance( childName );
    return false;
}

bool
EffectNode::createChild( quint32 typeId )
{
    QWriteLocker                        wl( &m_rwl );
    if ( m_plugin == NULL )
        return m_enf.createEffectNodeInstance( typeId );
    return false;
}

bool
EffectNode::createChild( const QString & typeName )
{
    QWriteLocker                        wl( &m_rwl );
    if ( m_plugin == NULL )
        return m_enf.createEffectNodeInstance( typeName );
    return false;
}

bool
EffectNode::deleteChild( quint32 childId )
{
    QWriteLocker                        wl( &m_rwl );
    if ( m_plugin == NULL )
        return m_enf.deleteEffectNodeInstance( childId );
    return false;
}

bool
EffectNode::deleteChild( const QString & childName )
{
    QWriteLocker                        wl( &m_rwl );
    if ( m_plugin == NULL )
        return m_enf.deleteEffectNodeInstance( childName );
    return false;
}

// ------------------- GETTING CHILDS -------------------

EffectNode*
EffectNode::getChild( quint32 childId ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_enf.getEffectNodeInstance( childId );
}

EffectNode*
EffectNode::getChild( const QString & childName ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_enf.getEffectNodeInstance( childName );
}

QList<EffectNode*>
EffectNode::getChildsList( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_enf.getEffectNodeInstancesList();
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

void
EffectNode::createStaticVideoInput( const QString & name )
{
    QWriteLocker                        wl( &m_rwl );
    m_staticVideosInputs.createObject( name );
    if ( m_plugin == NULL )
        m_internalsStaticVideosOutputs.createObject( name );

}

void
EffectNode::createStaticVideoOutput( const QString & name )
{
    QWriteLocker                        wl( &m_rwl );
    m_staticVideosOutputs.createObject( name );
    if ( m_plugin == NULL )
        m_internalsStaticVideosInputs.createObject( name );

}

//     void		addStaticAudioInput( QByteArray const & name );
//     void		addStaticAudioOutput( QByteArray const & name );
//     void		addStaticControlInput( QByteArray const & name );
//     void		addStaticControlOutput( QByteArray const & name );

void
EffectNode::createStaticVideoInput( void )
{
    QWriteLocker                        wl( &m_rwl );
    m_staticVideosInputs.createObject();
    if ( m_plugin == NULL )
        m_internalsStaticVideosOutputs.createObject();

}

void
EffectNode::createStaticVideoOutput( void )
{
    QWriteLocker                        wl( &m_rwl );
    m_staticVideosOutputs.createObject();
    if ( m_plugin == NULL )
        m_internalsStaticVideosInputs.createObject();

}
//     void		addStaticAudioInput( void );
//     void		addStaticAudioOutput( void );
//     void		addStaticControlInput( void );
//     void		addStaticControlOutput( void );


bool
EffectNode::removeStaticVideoInput( const QString & name )
{
    QWriteLocker                        wl( &m_rwl );
    if ( m_staticVideosInputs.deleteObject( name ) )
    {
        if ( m_plugin == NULL )
            if ( m_internalsStaticVideosOutputs.deleteObject( name ) == false )
                return false; // IF THIS CAS HAPPEND WE ARE SCREWED
        return true;
    }
    return false;
}

bool
EffectNode::removeStaticVideoOutput( const QString & name )
{
    QWriteLocker                        wl( &m_rwl );
    if ( m_staticVideosOutputs.deleteObject( name ) )
    {
        if ( m_plugin == NULL )
            if ( m_internalsStaticVideosInputs.deleteObject( name ) == false )
                return false; // IF THIS CAS HAPPEND WE ARE SCREWED
        return true;
    }
    return false;
}

//     bool		removeStaticAudioInput( QByteArray const & name );
//     bool		removeStaticAudioOutput( QByteArray const & name );
//     bool		removeStaticControlInput( QByteArray const & name );
//     bool		removeStaticControlOutput( QByteArray const & name );

bool
EffectNode::removeStaticVideoInput( quint32 id )
{
    QWriteLocker                        wl( &m_rwl );
    if ( m_staticVideosInputs.deleteObject( id ) )
    {
        if ( m_plugin == NULL )
            if ( m_internalsStaticVideosOutputs.deleteObject( id ) == false )
                return false; // IF THIS CAS HAPPEND WE ARE SCREWED
        return true;
    }
    return false;
}

bool
EffectNode::removeStaticVideoOutput( quint32 id )
{
    QWriteLocker                        wl( &m_rwl );
    if ( m_staticVideosOutputs.deleteObject( id ) )
    {
        if ( m_plugin == NULL )
            if ( m_internalsStaticVideosInputs.deleteObject( id ) == false )
                return false; // IF THIS CAS HAPPEND WE ARE SCREWED
        return true;
    }
    return false;
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

InSlot<LightVideoFrame>*
EffectNode::getStaticVideoInput( const QString & name ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_staticVideosInputs.getObject( name );
}

OutSlot<LightVideoFrame>*
EffectNode::getStaticVideoOutput( const QString & name ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_staticVideosOutputs.getObject( name );
}

//     InSlot<AudioSoundSample>*		getStaticAudioInput( QByteArray const & name ) const;
//     OutSlot<AudioSoundSample>*		getStaticAudioOutput( QByteArray const & name ) const;
//     InSlot<qreal>*		getStaticControlInput( QByteArray const & name ) const;
//     OutSlot<qreal>* 		getStaticControlOutput( QByteArray const & name ) const;

InSlot<LightVideoFrame>*
EffectNode::getStaticVideoInput( quint32 id ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_staticVideosInputs.getObject( id );
}

OutSlot<LightVideoFrame>*
EffectNode::getStaticVideoOutput( quint32 id ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_staticVideosOutputs.getObject( id );
}

//  InSlot<AudioSoundSample>*		getStaticAudioInput( quint32 id ) const;
//  OutSlot<AudioSoundSample>*		getStaticAudioOutput( quint32 id ) const;
//  InSlot<qreal>*		getStaticControlInput( quint32 id ) const;
//  OutSlot<qreal>*		getStaticControlOutput( quint32 id ) const;

QList<InSlot<LightVideoFrame>*>
EffectNode::getStaticsVideosInputsList( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_staticVideosInputs.getObjectsList();
}

QList<OutSlot<LightVideoFrame>*>
EffectNode::getStaticsVideosOutputsList( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_staticVideosOutputs.getObjectsList();
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

QList<QString>
EffectNode::getStaticsVideosInputsNamesList( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_staticVideosInputs.getObjectsNamesList();
}

QList<QString>
EffectNode::getStaticsVideosOutputsNamesList( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_staticVideosOutputs.getObjectsNamesList();
}

//     QList<QByteArray> const &	getStaticAudiosInputsNameList( void ) const;
//     QList<QByteArray> const &	getStaticAudiosOutputsNameList( void ) const;
//     QList<QByteArray> const &	getStaticControlsInputsNameList( void ) const;
//     QList<QByteArray> const &	getStaticControlsOutputsNameList( void ) const;

QList<quint32>
EffectNode::getStaticsVideosInputsIdsList( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_staticVideosInputs.getObjectsIdsList();
}

QList<quint32>
EffectNode::getStaticsVideosOutputsIdsList( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_staticVideosOutputs.getObjectsIdsList();
}

//     QList<QByteArray> const &	getStaticAudiosInputsIdList( void ) const;
//     QList<QByteArray> const &	getStaticAudiosOutputsIdList( void ) const;
//     QList<QByteArray> const &	getStaticControlsInputsIdList( void ) const;
//     QList<QByteArray> const &	getStaticControlsOutputsIdList( void ) const;

const QString
EffectNode::getStaticVideoInputNameById( quint32 id ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_staticVideosInputs.getObjectNameByObjectId( id );
}

const QString
EffectNode::getStaticVideoOutputNameById( quint32 id ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_staticVideosOutputs.getObjectNameByObjectId( id );
}

//     QByteArray const &          getStaticAudioInputNameById( quint32 const id ) const;
//     QByteArray const &          getStaticAudioOutputNameById( quint32 const id ) const;
//     QByteArray const &          getStaticControlInputNameById( quint32 const id ) const;
//     QByteArray const &          getStaticControlOutputNameById( quint32 const id ) const;

quint32
EffectNode::getStaticVideoInputIdByName( const QString & name ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_staticVideosInputs.getObjectIdByObjectName( name );
}

quint32
EffectNode::getStaticVideoOutputIdByName( const QString & name ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_staticVideosOutputs.getObjectIdByObjectName( name );
}

//     quint32                     getStaticAudioInputIdByName( QByteArray const & name ) const;
//     quint32                     getStaticAudioOutputIdByName( QByteArray const & name ) const;
//     quint32                     getStaticControlInputIdByName( QByteArray const & name ) const;
//     quint32                     getStaticControlOutputIdByName( QByteArray const & name ) const;

quint32
EffectNode::getNBStaticsVideosInputs( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_staticVideosInputs.getNBObjects();
}

quint32
EffectNode::getNBStaticsVideosOutputs( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_staticVideosOutputs.getNBObjects();
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


// -------------- GET INTERNALS ( JUST FOR EMPTY NODES) --------------


InSlot<LightVideoFrame>*
EffectNode::getInternalStaticVideoInput( const QString & name ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_internalsStaticVideosInputs.getObject( name );
}

OutSlot<LightVideoFrame>*
EffectNode::getInternalStaticVideoOutput( const QString & name ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_internalsStaticVideosOutputs.getObject( name );
}


InSlot<LightVideoFrame>*
EffectNode::getInternalStaticVideoInput( quint32 id ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_internalsStaticVideosInputs.getObject( id );
}

OutSlot<LightVideoFrame>*
EffectNode::getInternalStaticVideoOutput( quint32 id ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_internalsStaticVideosOutputs.getObject( id );
}


QList<InSlot<LightVideoFrame>*>
EffectNode::getInternalsStaticsVideosInputsList( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_internalsStaticVideosInputs.getObjectsList();
}

QList<OutSlot<LightVideoFrame>*>
EffectNode::getInternalsStaticsVideosOutputsList( void ) const
{
    QReadLocker                        rl( &m_rwl );
    return m_internalsStaticVideosOutputs.getObjectsList();
}

//-------------------------------------------------------------------------//
//      CONNECT/DISCONNECT & BIND/UNBIND DYN/STAT SLOTS TO PARENTS         //
//-------------------------------------------------------------------------//

// ----------------  CONNECT STATIC TO STATIC -------------------

bool
EffectNode::connectChildStaticVideoOutputToParentStaticVideoInput( const QString & childOutName,  const QString & fatherInName )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    InSlot<LightVideoFrame>*  in;

    if ( ( out = m_staticVideosOutputs.getObject( childOutName ) ) == NULL )
        return false;
    if ( m_father == NULL )
        return false;
    if ( ( in = m_father->getInternalStaticVideoInput( fatherInName ) ) == NULL )
        return false;
    if ( out->connect( *in ) == false )
        return false;
    if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
        return false;
    if ( m_father->referenceInternalStaticVideoInputAsConnected( in ) == false )
        return false;
    return true;
}

bool
EffectNode::connectChildStaticVideoOutputToParentStaticVideoInput( const QString & childOutName, quint32 fatherInId )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    InSlot<LightVideoFrame>*  in;

    if ( ( out = m_staticVideosOutputs.getObject( childOutName ) ) == NULL )
        return false;
    if ( m_father == NULL )
        return false;
    if ( ( in = m_father->getInternalStaticVideoInput( fatherInId ) ) == NULL )
        return false;
    if ( out->connect( *in ) == false )
        return false;
    if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
        return false;
    if ( m_father->referenceInternalStaticVideoInputAsConnected( in ) == false )
        return false;
    return true;
}

bool
EffectNode::connectChildStaticVideoOutputToParentStaticVideoInput( quint32 childOutId, const QString & fatherInName )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    InSlot<LightVideoFrame>*  in;

    if ( ( out = m_staticVideosOutputs.getObject( childOutId ) ) == NULL )
        return false;
    if ( m_father == NULL )
        return false;
    if ( ( in = m_father->getInternalStaticVideoInput( fatherInName ) ) == NULL )
        return false;
    if ( out->connect( *in ) == false )
        return false;
    if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
        return false;
    if ( m_father->referenceInternalStaticVideoInputAsConnected( in ) == false )
        return false;
    return true;
}

bool
EffectNode::connectChildStaticVideoOutputToParentStaticVideoInput( quint32 childOutId, quint32 fatherInId )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    InSlot<LightVideoFrame>*  in;

    if ( ( out = m_staticVideosOutputs.getObject( childOutId ) ) == NULL )
        return false;
    if ( m_father == NULL )
        return false;
    if ( ( in = m_father->getInternalStaticVideoInput( fatherInId ) ) == NULL )
        return false;
    if ( out->connect( *in ) == false )
        return false;
    if ( m_connectedStaticVideosOutputs.addObjectReference( out ) == false )
        return false;
    if ( m_father->referenceInternalStaticVideoInputAsConnected( in ) == false )
        return false;
    return true;
}

bool
EffectNode::connectChildStaticVideoInputToParentStaticVideoOutput( const QString & childInName,  const QString & fatherOutName )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    InSlot<LightVideoFrame>*  in;

    if ( ( in = m_staticVideosInputs.getObject( childInName ) ) == NULL )
        return false;
    if ( m_father == NULL )
        return false;
    if ( ( out = m_father->getInternalStaticVideoOutput( fatherOutName ) ) == NULL )
        return false;
    if ( out->connect( *in ) == false )
        return false;
    if ( m_connectedStaticVideosInputs.addObjectReference( in ) == false )
        return false;
    if ( m_father->referenceInternalStaticVideoOutputAsConnected( out ) == false )
        return false;
    return true;
}

bool
EffectNode::connectChildStaticVideoInputToParentStaticVideoOutput( const QString & childInName, quint32 fatherOutId )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    InSlot<LightVideoFrame>*  in;

    if ( ( in = m_staticVideosInputs.getObject( childInName ) ) == NULL )
        return false;
    if ( m_father == NULL )
        return false;
    if ( ( out = m_father->getInternalStaticVideoOutput( fatherOutId ) ) == NULL )
        return false;
    if ( out->connect( *in ) == false )
        return false;
    if ( m_connectedStaticVideosInputs.addObjectReference( in ) == false )
        return false;
    if ( m_father->referenceInternalStaticVideoOutputAsConnected( out ) == false )
        return false;
    return true;
}

bool
EffectNode::connectChildStaticVideoInputToParentStaticVideoOutput( quint32 childInId, const QString & fatherOutName )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    InSlot<LightVideoFrame>*  in;

    if ( ( in = m_staticVideosInputs.getObject( childInId ) ) == NULL )
        return false;
    if ( m_father == NULL )
        return false;
    if ( ( out = m_father->getInternalStaticVideoOutput( fatherOutName ) ) == NULL )
        return false;
    if ( out->connect( *in ) == false )
        return false;
    if ( m_connectedStaticVideosInputs.addObjectReference( in ) == false )
        return false;
    if ( m_father->referenceInternalStaticVideoOutputAsConnected( out ) == false )
        return false;
    return true;
}

bool
EffectNode::connectChildStaticVideoInputToParentStaticVideoOutput( quint32 childInId, quint32 fatherOutId )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    InSlot<LightVideoFrame>*  in;

    if ( ( in = m_staticVideosInputs.getObject( childInId ) ) == NULL )
        return false;
    if ( m_father == NULL )
        return false;
    if ( ( out = m_father->getInternalStaticVideoOutput( fatherOutId ) ) == NULL )
        return false;
    if ( out->connect( *in ) == false )
        return false;
    if ( m_connectedStaticVideosInputs.addObjectReference( in ) == false )
        return false;
    if ( m_father->referenceInternalStaticVideoOutputAsConnected( out ) == false )
        return false;
    return true;
}

// ---------------- INTERNALS SLOTS DISCONNECTS --------------------

bool
EffectNode::disconnectInternalStaticVideoOutput( quint32 nodeId )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    InSlot<LightVideoFrame>*   in;
    EffectNode*                father;

    if ( ( out = m_internalsStaticVideosOutputs.getObject( nodeId ) ) == NULL )
        return false;
    in = out->getInSlotPtr();
    if ( out->disconnect() == false )
        return false;
    if ( m_connectedInternalsStaticVideosOutputs.delObjectReference( out->getId() ) == false )
        return false;
    father = in->getPrivateFather();
    if ( father->dereferenceStaticVideoInputAsConnected( in->getId() ) == false )
        return false;
    return true;
}

bool
EffectNode::disconnectInternalStaticVideoOutput( const QString & nodeName )
{
    QWriteLocker                        wl( &m_rwl );
    OutSlot<LightVideoFrame>*  out;
    InSlot<LightVideoFrame>*   in;
    EffectNode*                father;

    if ( ( out = m_internalsStaticVideosOutputs.getObject( nodeName ) ) == NULL )
        return false;
    in = out->getInSlotPtr();
    if ( out->disconnect() == false )
        return false;
    if ( m_connectedInternalsStaticVideosOutputs.delObjectReference( out->getId() ) == false )
        return false;
    father = in->getPrivateFather();
    if ( father->dereferenceStaticVideoInputAsConnected( in->getId() ) == false )
        return false;
    return true;
}


//-------------------------------------------------------------------------//
//                     CONNECTED SLOTS MAP MANAGEMENT                      //
//-------------------------------------------------------------------------//


bool
EffectNode::referenceStaticVideoInputAsConnected( InSlot<LightVideoFrame>* in )
{
    QWriteLocker                        wl( &m_rwl );

    return m_connectedStaticVideosInputs.addObjectReference( in );
}

bool
EffectNode::referenceInternalStaticVideoOutputAsConnected( OutSlot<LightVideoFrame>* out )
{
    QWriteLocker                        wl( &m_rwl );

    return m_connectedInternalsStaticVideosOutputs.addObjectReference( out );
}

bool
EffectNode::referenceStaticVideoOutputAsConnected( OutSlot<LightVideoFrame>* out )
{
    QWriteLocker                        wl( &m_rwl );

    return m_connectedStaticVideosOutputs.addObjectReference( out );
}

bool
EffectNode::referenceInternalStaticVideoInputAsConnected( InSlot<LightVideoFrame>* in )
{
    QWriteLocker                        wl( &m_rwl );

    return m_connectedInternalsStaticVideosInputs.addObjectReference( in );
}

bool
EffectNode::dereferenceStaticVideoInputAsConnected( quint32 inId )
{
    QWriteLocker                        wl( &m_rwl );

    return m_connectedStaticVideosInputs.delObjectReference( inId );
}

bool
EffectNode::dereferenceInternalStaticVideoOutputAsConnected( quint32 outId )
{
    QWriteLocker                        wl( &m_rwl );

    return m_connectedInternalsStaticVideosOutputs.delObjectReference(  outId );
}

bool
EffectNode::dereferenceStaticVideoOutputAsConnected( quint32 outId )
{
    QWriteLocker                        wl( &m_rwl );

    return m_connectedStaticVideosOutputs.delObjectReference(  outId );
}

bool
EffectNode::dereferenceInternalStaticVideoInputAsConnected( quint32 inId )
{
    QWriteLocker                        wl( &m_rwl );

    return m_connectedInternalsStaticVideosInputs.delObjectReference( inId );
}

QList<InSlot<LightVideoFrame>*>
EffectNode::getConnectedStaticsVideosInputsList( void ) const
{
    QReadLocker                        rl( &m_rwl );

    return m_connectedStaticVideosInputs.getObjectsReferencesList();
}

QList<OutSlot<LightVideoFrame>*>
EffectNode::getConnectedInternalsStaticsVideosOutputsList( void ) const
{
    QReadLocker                        rl( &m_rwl );

    return m_connectedInternalsStaticVideosOutputs.getObjectsReferencesList();
}

QList<OutSlot<LightVideoFrame>*>
EffectNode::getConnectedStaticsVideosOutputsList( void ) const
{
    QReadLocker                        rl( &m_rwl );

    return m_connectedStaticVideosOutputs.getObjectsReferencesList();
}

QList<InSlot<LightVideoFrame>*>
EffectNode::getConnectedInternalsStaticsVideosInputsList( void ) const
{
    QReadLocker                        rl( &m_rwl );

    return m_connectedInternalsStaticVideosInputs.getObjectsReferencesList();
}

quint32
EffectNode::getNBConnectedStaticsVideosInputs( void ) const
{
    QReadLocker                        rl( &m_rwl );

    return m_connectedStaticVideosInputs.getNBObjectsReferences();
}

quint32
EffectNode::getNBConnectedInternalsStaticsVideosOutputs( void ) const
{
    QReadLocker                        rl( &m_rwl );

    return m_connectedInternalsStaticVideosOutputs.getNBObjectsReferences();
}

quint32
EffectNode::getNBConnectedStaticsVideosOutputs( void ) const
{
    QReadLocker                        rl( &m_rwl );

    return m_connectedStaticVideosOutputs.getNBObjectsReferences();
}

quint32
EffectNode::getNBConnectedInternalsStaticsVideosInputs( void ) const
{
    QReadLocker                        rl( &m_rwl );

    return m_connectedInternalsStaticVideosInputs.getNBObjectsReferences();
}
