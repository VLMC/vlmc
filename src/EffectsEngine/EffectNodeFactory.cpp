/*****************************************************************************
 * EffectNodeFactory.cpp: this class is used to instantiate a new EffectNode
 *                        which contains a plugin effect
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

#include "EffectNodeFactory.h"

#include "EffectNode.h"
#include "EffectPluginTypeManager.h"
#include "IEffectPluginCreator.h"

EffectPluginTypeManager EffectNodeFactory::s_eptm = EffectPluginTypeManager();

EffectNodeFactory::EffectNodeFactory( void ) : m_higherFreeId( 1 ),
                                         m_mapHoles( 0 ),
                                         m_father( NULL )
{
}

EffectNodeFactory::~EffectNodeFactory()
{
    if ( m_enByName.empty() == false )
    {
        QMap<QString, EffectNode*>::iterator it  = m_enByName.begin();;
        QMap<QString, EffectNode*>::iterator end = m_enByName.end();

        for ( ; it != end; ++it )
            delete it.value();
    }
}


// SETTING FATHER

void
EffectNodeFactory::setFather( EffectNode* father )
{
    m_father = father ;
}

// EFFECT TYPES INFORMATION

QList<QString>
EffectNodeFactory::getEffectNodeTypesNamesList( void ) const
{
    return EffectNodeFactory::s_eptm.getEffectPluginTypesNamesList();
}

QList<quint32>
EffectNodeFactory::getEffectNodeTypesIdsList( void ) const
{
    return EffectNodeFactory::s_eptm.getEffectPluginTypesIdsList();
}

const QString
EffectNodeFactory::getEffectNodeTypeNameByTypeId( quint32 typeId ) const
{
    return EffectNodeFactory::s_eptm.getEffectPluginTypeNameByTypeId( typeId );
}

quint32
EffectNodeFactory::getEffectNodeTypeIdByTypeName( const QString & typeName ) const
{
    return EffectNodeFactory::s_eptm.getEffectPluginTypeIdByTypeName( typeName );
}

// EFFECT INSTANCES INFORMATIONS

QList<QString>
EffectNodeFactory::getEffectNodeInstancesNamesList( void ) const
{
    return m_nameById.values();
}

QList<quint32>
EffectNodeFactory::getEffectNodeInstancesIdsList( void ) const
{
    return m_nameById.keys();
}

const QString
EffectNodeFactory::getEffectNodeInstanceNameByInstanceId( quint32 instanceId ) const
{
    return m_nameById.value( instanceId, "" );
}

quint32
EffectNodeFactory::getEffectNodeInstanceIdByInstanceName( const QString & instanceName ) const
{
    return m_nameById.key( instanceName, 0 );
}

// CREATE AND DELETE EFFECTS

void
EffectNodeFactory::createEmptyEffectNodeInstance( void )
{
    EffectNode*         newNode;
    quint32             instanceId;
    QString             instanceName;
    QString             typeName("EmptyEffectNode");
    quint32             typeId = 1;

    newNode = new EffectNode();
    if ( m_mapHoles > 0 )
    {
        instanceId = m_enById.key( NULL );
        --m_mapHoles;
    }
    else
    {
        instanceId = m_higherFreeId;
        ++m_higherFreeId;
    }
    instanceName.setNum( instanceId );
    instanceName.insert( 0 , typeName + "_" );
    newNode->setTypeId( typeId );
    newNode->setTypeName( typeName );
    newNode->setInstanceId( instanceId );
    newNode->setInstanceName( instanceName );
    newNode->setFather( m_father );
    m_enByName[ instanceName ] = newNode;
    m_enById[ instanceId ] = newNode;
    m_nameById[ instanceId ] = instanceName;
    qDebug() << "New empty EffectNode* created with name : " << instanceName ;
}

bool
EffectNodeFactory::createEmptyEffectNodeInstance( const QString & instanceName )
{
    if ( m_enByName.find( instanceName ) == m_enByName.end() )
    {
        EffectNode*         newNode;
        quint32             instanceId;
        QString             typeName("EmptyEffectNode");
        quint32             typeId = 1;

        newNode = new EffectNode();
        if ( m_mapHoles > 0 )
        {
            instanceId = m_enById.key( NULL );
            --m_mapHoles;
        }
        else
        {
            instanceId = m_higherFreeId;
            ++m_higherFreeId;
        }
        newNode->setTypeId( typeId );
        newNode->setTypeName( typeName );
        newNode->setInstanceId( instanceId );
        newNode->setInstanceName( instanceName );
        newNode->setFather( m_father );
        m_enByName[ instanceName ] = newNode;
        m_enById[ instanceId ] = newNode;
        m_nameById[ instanceId ] = instanceName;
        qDebug() << "New empty EffectNode* created with name : " << instanceName ;
        return true;
    }
    qDebug() << "You can't create a new empty EffectNode with name : "
             << instanceName << ", it already exist!";
    return false;
}

bool
EffectNodeFactory::createEffectNodeInstance( const QString & typeName )
{
    IEffectPlugin*      newInstance;
    EffectNode*         newNode;
    quint32             instanceId;
    QString             instanceName;

    newInstance = EffectNodeFactory::s_eptm.createIEffectPluginInstance( typeName );
    if ( newInstance != NULL )
    {
        newNode = new EffectNode( newInstance );
        if ( m_mapHoles > 0 )
        {
            instanceId = m_enById.key( NULL );
            --m_mapHoles;
        }
        else
        {
            instanceId = m_higherFreeId;
            ++m_higherFreeId;
        }
        instanceName.setNum( instanceId );
        instanceName.insert( 0 , typeName + "_" );
        newNode->setTypeId( getEffectNodeTypeIdByTypeName( typeName) );
        newNode->setTypeName( typeName );
        newNode->setInstanceId( instanceId );
        newNode->setInstanceName( instanceName );
        newNode->setFather( m_father );
        m_enByName[ instanceName ] = newNode;
        m_enById[ instanceId ] = newNode;
        m_nameById[ instanceId ] = instanceName;
        return true;
        qDebug() << "EffectNode* with typeName[" << typeName << "] created.";
    }
    qDebug() << "Can't create EffectNode* with typeName[" << typeName << "]!";
    return false;
}

bool
EffectNodeFactory::createEffectNodeInstance( quint32 typeId )
{
    IEffectPlugin*      newInstance;
    EffectNode*         newNode;
    quint32             instanceId;
    QString             instanceName;

    newInstance = EffectNodeFactory::s_eptm.createIEffectPluginInstance( typeId );
    if ( newInstance != NULL )
    {
        newNode = new EffectNode( newInstance );
        if ( m_mapHoles > 0 )
        {
            instanceId = m_enById.key( NULL );
            --m_mapHoles;
        }
        else
        {
            instanceId = m_higherFreeId;
            ++m_higherFreeId;
        }
        instanceName.setNum( instanceId );
        instanceName.insert( 0 , getEffectNodeTypeNameByTypeId( typeId ) + "_" );
        newNode->setTypeId( typeId );
        newNode->setTypeName( getEffectNodeTypeNameByTypeId( typeId ) );
        newNode->setInstanceId( instanceId );
        newNode->setInstanceName( instanceName );
        newNode->setFather( m_father );
        m_enByName[ instanceName ] = newNode;
        m_enById[ instanceId ] = newNode;
        m_nameById[ instanceId ] = instanceName;
        return true;
        qDebug() << "EffectNode* with typeId[" << typeId << "] created.";
    }
    qDebug() << "Can't create EffectNode* with typeId[" << typeId << "]!";
    return false;
}

bool        EffectNodeFactory::deleteEffectNodeInstance( const QString & instanceName )
{
    QMap<quint32, EffectNode*>::iterator itid;
    quint32                             instanceId;

    itid = m_enById.find( getEffectNodeInstanceIdByInstanceName( instanceName ) );

    if ( itid != m_enById.end() )
    {
        EffectNode*      value;

        value = itid.value();
        instanceId = itid.key();
        if (value != NULL)
        {
            delete value;

            m_enByName.erase( m_enByName.find( instanceName ) );
            m_nameById.erase( m_nameById.find( instanceId) );
            if ( instanceId <  ( m_higherFreeId - 1 ) )
            {
                m_enById[ instanceId ] = NULL;
                ++m_mapHoles;
            }
            else
            {
                m_enById.erase( itid );
                --m_higherFreeId;
            }

        }
        else
        {
            qDebug() << "You can't delete the EffectNode* instance with instanceName["
                     << instanceName
                     << "] it already has been deleted!";
            return false;
        }
    }
    else
    {
        qDebug() << "You can't delete the EffectNode* instance with instanceName["
                 << instanceName
                 << "] it doesn't exist!";
        return false;
    }
    return true;
}

bool
EffectNodeFactory::deleteEffectNodeInstance( quint32 instanceId )
{
    QMap<quint32, EffectNode*>::iterator itid;

    itid = m_enById.find( instanceId );

    if ( itid != m_enById.end() )
    {
        EffectNode*      value;

        value = itid.value();
        if (value != NULL)
        {
            delete value;

            m_enByName.erase( m_enByName.find( getEffectNodeInstanceNameByInstanceId( instanceId ) ) );
            m_nameById.erase( m_nameById.find( instanceId) );
            if ( instanceId <  ( m_higherFreeId - 1 ) )
            {
                m_enById[ instanceId ] = NULL;
                ++m_mapHoles;
            }
            else
            {
                m_enById.erase( itid );
                --m_higherFreeId;
            }

        }
        else
        {
            qDebug() << "You can't delete the EffectNode* instance with instanceId["
                     << instanceId
                     << "] it already has been deleted!";
            return false;
        }
    }
    else
    {
        qDebug() << "You can't delete the EffectNode* instance with instanceId["
                 << instanceId
                 << "] it doesn't exist!";
        return false;
    }
    return true;
}

EffectNode*
EffectNodeFactory::getEffectNodeInstance( quint32 instanceId ) const
{
   QMap<quint32, EffectNode*>::const_iterator    it = m_enById.find( instanceId );

    if ( it != m_enById.end() )
        return it.value();
    return NULL;
}

EffectNode*
EffectNodeFactory::getEffectNodeInstance( const QString & instanceName ) const
{
    QMap<QString, EffectNode*>::const_iterator    it = m_enByName.find( instanceName );

    if ( it != m_enByName.end() )
        return it.value();
    return NULL;
}

QList<EffectNode*>  EffectNodeFactory::getEffectNodeInstancesList( void ) const
{
    return m_enByName.values();
}
