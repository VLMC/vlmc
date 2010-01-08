/*****************************************************************************
 * SimpleObjectsReferencer.h: Template class use to store obj reference
 *                            ( not cpp refs, in internal it's pointers).
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

#ifndef SIMPLEOBJECTSREFERENCER_H_
#define SIMPLEOBJECTSREFERENCER_H_

#include <QtGlobal>
#include <QMap>

template<typename T>
class      SimpleObjectsReferencer
{
public:

    SimpleObjectsReferencer( void )
    {};

    ~SimpleObjectsReferencer()
    {}

    bool     addObjectReference( T * obj )
    {
        if ( m_objRefs.find( obj->getId() ) == m_objRefs.end() )
        {
            m_objRefs[ obj->getId() ] = obj;
            return true;
        }
        return false;
    };

    bool     delObjectReference( quint32 objId )
    {
        typename QMap<quint32, T*>::iterator it = m_objRefs.find( objId );

        if ( it != m_objRefs.end() )
        {
            m_objRefs.erase( it );
            return true;
        }
        return false;
    };

    quint32 getNBObjectsReferences( void ) const
    {
        return m_objRefs.size();
    };

    QList<T*> getObjectsReferencesList( void ) const
    {
        return m_objRefs.values();
    };

private:

    QMap<quint32, T*>                 m_objRefs;
};

#endif // SIMPLEOBJECTSREFERENCER_H_
