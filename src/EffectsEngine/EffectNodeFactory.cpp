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

EffectNodeFactory::EffectNodeFactory() : m_higherFreeId( 1 )
{
}

EffectNodeFactory::~EffectNodeFactory()
{
    if ( m_enById.empty() == false )
    {
        quint32     i;
        quint32     size;

        size = m_enById.size();
        for ( i = 0; i < size; ++i )
            delete m_enById[i];
    }
}

// EFFECT TYPES INFORMATION

QList<QString> const &      EffectNodeFactory::getEffectNodeTypesNamesList( void ) const
{
    return ( m_eptm::getEffectPluginTypesNamesList() );
}

QList<quint32> const &      EffectNodeFactory::getEffectNodeTypesIdsList( void ) const
{
    return ( m_eptm::getEffectPluginTypesIdsList() );
}

QString const               EffectNodeFactory::getEffectNodeTypeNameByTypeId( quint32 typeId ) const
{
    return ( m_eptm::getEffectPluginTypeNameByTypeId( typeId ) );
}

quint32                     EffectNodeFactory::getEffectNodeTypeIdByTypeName( QString const & typeName ) const
{
    return ( m_eptm::getEffectPluginTypeIdByTypeName( typeName ) );
}

// EFFECT INSTANCES INFORMATIONS

QList<QString> const &      EffectNodeFactory::getEffectNodeInstancesNamesList( void ) const
{
    return ( m_nameById.values() );
}

QList<quint32> const &      EffectNodeFactory::getEffectNodeInstancesIdsList( void ) const
{
    return ( m_nameById.keys() );
}

QString const               EffectNodeFactory::getEffectNodeInstanceNameByInstanceId( quint32 typeId ) const
{
    return ( m_nameById.value( typeId, "" ) );
}

quint32                     EffectNodeFactory::getEffectNodeInstanceIdByInstanceName( QString const & typeName ) const
{
    return ( m_nameById.key( typeName, 0 ) );
}

// CREATE AND DELETE EFFECTS

bool                        EffectNodeFactory::createEffectNodeInstance( QString const & typeName )
{
    IEffectPlugin*      newInstance;
    EffectNode*         newNode;
    quint32             instanceId;
    QString             instanceName;

    newInstance = m_eptm::createIEffectPluginInstance( typeName );
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
        newNode.setTypeId( getEffectNodeTypeIdByTypeName( typeName) );
        newNode.setTypeName( typeName );
        newNode.setInstanceId( instanceId );
        newNode.setInstanceName( instanceName );
        m_enByName[ instanceName ] = newNode;
        m_enById[ instanceId ] = newNode;
        m_nameById[ instanceId ] = instanceName;
        return ( true );
    }
    return ( false );
}

bool        EffectNodeFactory::createEffectNodeInstance( quint32 typeId )
{
    IEffectPlugin*      newInstance;
    EffectNode*         newNode;
    quint32             instanceId;
    QString             instanceName;

    newInstance = m_eptm::createIEffectPluginInstance( typeId );
    if ( newInstace != NULL )
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
        newNode.setTypeId( typeId );
        newNode.setTypeName( getEffectNodeTypeNameByTypeId( typeId ) );
        newNode.setInstanceId( instanceId );
        newNode.setInstanceName( instanceName );
        m_enByName[ instanceName ] = newNode;
        m_enById[ instanceId ] = newNode;
        m_nameById[ instanceId ] = instanceName;
        return ( true );
    }
    return ( false );
}

bool        EffectNodeFactory::deleteEffectNodeInstance( QString const & instanceName )
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
            m_nameBydId.erase( m_nameById.find( instanceId) );
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
            return ( false );
        }
    }
    else
    {
        qDebug() << "You can't delete the EffectNode* instance with instanceName["
                 << instanceName
                 << "] it doesn't exist!";
        return ( false );
    }
    return ( true );
}

bool        EffectNodeFactory::deleteEffectNodeInstance( quint32 instanceId )
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

            m_enByName.erase( m_enByName.find( getEffectNodeInstanceNameById( instanceId ) ) );
            m_nameBydId.erase( m_nameById.find( instanceId) );
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
            return ( false );
        }
    }
    else
    {
        qDebug() << "You can't delete the EffectNode* instance with instanceId["
                 << instanceId
                 << "] it doesn't exist!";
        return ( false );
    }
    return ( true );
}

EffectNode* EffectNodeFactory::getEffectNodeInstance( quint32 instanceId ) const
{
   QMap<quint32, EffectNode*>::iterator    it;

    if ( ( it = m_enById.find( instanceId ) ) == m_enById.end() )
        return ( it.value() );
    return ( NULL );
}

EffectNode* EffectNodeFactory::getEffectNodeInstance( QString const & instanceName ) const;
{
   QMap<QString, EffectNode*>::iterator    it;

    if ( ( it = m_enByName.find( instanceName ) ) == m_enByName.end() )
        return ( it.value() );
    return ( NULL );
}

QList<EffectNode*>  EffectNodeFactory::getEffectNodeInstancesList( void ) const
{
    return ( m_enById.values() );
}
