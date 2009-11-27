/*****************************************************************************
 * EffectPluginFactory.cpp: this class is used to load a .so and instantiate
 *                          a new IEffectPlugin from this
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

#include "EffectPluginFactory.h"

EffectPluginFactory::EffectPluginFactory() : m_iepc( NULL ), m_higherFreeId( 0 ), mapHoles( 0 )
{
}

EffectPluginFactory::~EffectPluginFactory()
{
}

bool    EffectPluginFactory::load( QString const & fileName )
{
    QObject*    tmp;
    m_qpl.setFileName( fileName );
    tmp = m_qpl.instance();

    if ( tmp == NULL )
    {
        QDebug() << m_qpl.errorString();
        return ( false );
    }

    m_iepc = qobject_cast<IEffectPluginCreator*>( tmp );
    if ( m_iepc == NULL )
    {
        QDebug() << "The type of the created instance of the loaded class isn't IEffectPluginCreator* !";
        return ( false );
    }
    return  ( true );
}

IEffectPlugin*  EffectPluginFactory::newIEffectPlugin( void )
{
    if ( m_iepc != NULL )
    {
        IEffectPlugin*      newInstance;

        newInstance = m_iepc->getInstance();
        if ( mapHoles > 0 )
        {
            m_iepi[m_iepi.key( NULL )] = newInstance;
            --mapHoles;
        }
        else
        {
            m_iepi[m_higherFreeId] = newInstance;
            ++m_higherFreeId;
        }
        return ( newInstance );
    }
    QDebug() << "You must load the [filename].so before trying to instantiate an IEffectPlugin!";
    return ( NULL );
}

bool            EffectPluginFactory::deleteIEffectPluginInstance( quint32 id )
{
    if (m_iepc != NULL)
    {
        QMap<quint32, IEffectPlugin*>::iterator it;

        it = m_iepi.find( id );
        if ( it != m_iepi.end() )
        {
            IEffectPlugin*      value;

            value = it.value();
            if (value != NULL)
            {
                m_iepc->deleteInstance( value );
                if ( id <  ( m_higherFreeId - 1 ) )
                {
                    m_iepi[ id ] = NULL
                    ++m_mapHoles;
                }
                else
                {
                    m_iepi.erase( id );
                    --m_higherFreeId;
                }
            }
            else
            {
                qDebug() << "You can't delete the IEffectPlugin* instance with id["
                         << id
                         << "] it already has been deleted!";
            }
        }
        else
        {
            qDebug() << "You can't delete the IEffectPlugin* instance with id["
                     << id
                     << "] it doesn't exist!";
        }
    }
    qDebug() << "You must load the [filename].so and instantiate an IEffectPlugin before trying to delete an instance!";
    return ( false );
}
