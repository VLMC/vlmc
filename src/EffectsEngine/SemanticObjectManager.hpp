/*****************************************************************************
 * SemanticObjectManager.hpp: Template class providing object managing (creation,
 *                            deletion, fetching, informations) by names
 *                            (so with semantic) and by ids
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

#ifndef SEMANTICOBJECTMANAGER_H_
#define SEMANTICOBJECTMANAGER_H_

#include <QString>
#include <QMap>

class EffectNode;

template<typename T>
class SemanticObjectManager
{
public:

    SemanticObjectManager( void ) : m_higherFreeId( 1 ),
                                    m_mapHoles( 0 ),
                                    m_father( NULL ),
                                    m_isItInternal( false )
    {
    }

    ~SemanticObjectManager()
    {
        if ( m_objectByName.empty() == false )
        {
            typename QMap<QString, T*>::iterator it = m_objectByName.begin();
            typename QMap<QString, T*>::iterator end = m_objectByName.end();

            for ( ; it != end; ++it )
                delete it.value();
        }
    }

    inline void setFather( EffectNode* father)
    {
        m_father = father;
    }

    inline void setScope( bool isItInternal )
    {
        m_isItInternal = isItInternal;
    }

    // OBJECTS INFORMATIONS

    inline QList<QString> getObjectsNamesList( void ) const
    {
        return m_nameById.values();
    }

    inline QList<quint32> getObjectsIdsList( void ) const
    {
        return m_nameById.keys();
    }

    inline const QString               getObjectNameByObjectId( quint32 objectId ) const
    {
        return m_nameById.value( objectId, "" );
    }

    inline quint32                     getObjectIdByObjectName( const QString & objectName ) const
    {
        return m_nameById.key( objectName, 0 );
    }

    inline quint32                     getNBObjects( void ) const
    {
        return m_nameById.size();
    }

    // CREATE AND DELETE OBJECTS

    inline void                        createObject( void )
    {
        T*                          newObject;
        quint32                     objectId;
        QString                     objectName;

        newObject = new T;
        if ( m_mapHoles > 0 )
        {
            objectId = m_objectById.key( NULL );
            --m_mapHoles;
        }
        else
        {
            objectId = m_higherFreeId;
            ++m_higherFreeId;
        }
        objectName.setNum( objectId );
        objectName.insert( 0, "Number_" );

        newObject->setId( objectId );
        newObject->setName( objectName );
        newObject->setFather( m_father );
        newObject->setScope( m_isItInternal );

        m_objectByName[ objectName ] = newObject;
        m_objectById[ objectId ] = newObject;
        m_nameById[ objectId ] = objectName;
    }

    inline void                        createObject( const QString & objectName )
    {
        T*                          newObject;
        quint32                     objectId;

        newObject = new T;
        if ( m_mapHoles > 0 )
        {
            objectId = m_objectById.key( NULL );
            --m_mapHoles;
        }
        else
        {
            objectId = m_higherFreeId;
            ++m_higherFreeId;
        }

        newObject->setId( objectId );
        newObject->setName( objectName );
        newObject->setFather( m_father );
        newObject->setScope( m_isItInternal );

        m_objectByName[ objectName ] = newObject;
        m_objectById[ objectId ] = newObject;
        m_nameById[ objectId ] = objectName;
    }

    inline bool                        deleteObject( quint32 objectId )
    {
        typename QMap<quint32, T*>::iterator itid;

        itid = m_objectById.find( objectId );

        if ( itid != m_objectById.end() )
        {
            T*      value;

            value = itid.value();
            objectId = itid.key();
            if (value != NULL)
            {
                delete value;

                m_objectByName.erase( m_objectByName.find( getObjectNameByObjectId( objectId ) ) );
                m_nameById.erase( m_nameById.find( objectId) );
                if ( objectId <  ( m_higherFreeId - 1 ) )
                {
                    m_objectById[ objectId ] = NULL;
                    ++m_mapHoles;
                }
                else
                {
                    m_objectById.erase( itid );
                    --m_higherFreeId;
                }
            }
            else
            {
                qDebug() << "You can't delete the object with ["
                         << objectId
                         << "] as id, it already has been deleted!";
                return false;
            }
        }
        else
        {
            qDebug() << "You can't delete the object with ["
                     << objectId
                     << "] as id, it doesn't exist!";
            return false;
        }
        return true;
    }

    inline bool                        deleteObject( const QString & objectName )
    {
        typename QMap<quint32, T*>::iterator itid;
        quint32                     objectId;

        itid = m_objectById.find( getObjectIdByObjectName( objectName ) );

        if ( itid != m_objectById.end() )
        {
            T*      value;

            value = itid.value();
            objectId = itid.key();
            if (value != NULL)
            {
                delete value;

                m_objectByName.erase( m_objectByName.find( objectName ) );
                m_nameById.erase( m_nameById.find( objectId) );
                if ( objectId <  ( m_higherFreeId - 1 ) )
                {
                    m_objectById[ objectId ] = NULL;
                    ++m_mapHoles;
                }
                else
                {
                    m_objectById.erase( itid );
                    --m_higherFreeId;
                }
            }
            else
            {
                qDebug() << "You can't delete the object named ["
                         << objectName
                         << "], it already has been deleted!";
                return false;
            }
        }
        else
        {
            qDebug() << "You can't delete the object named ["
                     << objectName
                     << "], it doesn't exist!";
            return false;
        }
        return true;
    }

    // GETTING OBJECTS

    inline T*                          getObject( quint32 objectId ) const
    {
        typename QMap<quint32, T*>::const_iterator    it = m_objectById.find( objectId );

        if ( it != m_objectById.end() )
            return it.value();
        return NULL;
    }

    inline T*                          getObject( const QString & objectName ) const
    {
        typename QMap<QString, T*>::const_iterator    it = m_objectByName.find( objectName );

        if ( it != m_objectByName.end() )
            return it.value();
        return NULL;
    }

    inline QList<T*>                   getObjectsList( void ) const
    {
        return m_objectByName.values();
    }

private:

    QMap<quint32, T*>           m_objectById;
    QMap<QString, T*>           m_objectByName;
    QMap<quint32, QString>      m_nameById;
    quint32                     m_higherFreeId;
    quint32                     m_mapHoles;
    EffectNode*                 m_father;
    bool                        m_isItInternal;
};

#endif // SEMANTICOBJECTMANAGER_H_
