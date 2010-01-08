/*****************************************************************************
 * EffectPluginTypeLoader.cpp: this class is used to load a .so and instantiate
 *                             the IEffectPluginCreator from this
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

#include "EffectPluginTypeLoader.h"

#include <IEffectPluginCreator.h>

#include <QDebug>

EffectPluginTypeLoader::EffectPluginTypeLoader( void ) : m_iepc( NULL )
{
}

EffectPluginTypeLoader::~EffectPluginTypeLoader()
{
}

IEffectPlugin*   EffectPluginTypeLoader::createIEffectPluginInstance( void ) const
{
    if ( m_iepc != NULL )
        return m_iepc->createIEffectPluginInstance();
    return NULL;
}

bool    EffectPluginTypeLoader::load( const QString & fileName )
{
    QObject*    tmp;
    m_qpl.setFileName( fileName );
    tmp = m_qpl.instance();

    if ( tmp == NULL )
    {
        qDebug() << m_qpl.errorString();
        return false ;
    }

    m_iepc = qobject_cast<IEffectPluginCreator*>( tmp );
    if ( m_iepc == NULL )
    {
        qDebug() << "The type of the created instance of"
                 << "the loaded class isn't IEffectPluginCreator* !";
        return false;
    }
    return true;
}
