/*****************************************************************************
 * InSlot.hpp: InSlot is for getting entering data from OutSlot for
 * inter-effects communication
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

#ifndef INSLOT_HPP_
#define INSLOT_HPP_

#include <QDebug>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>

class EffectNode;
class IEffectNode;

template<typename T> class OutSlot;


template<typename T>
class	InSlot
{
    friend class OutSlot<T>;

public:

    // CTOR & DTOR

    InSlot( void );
    InSlot( const InSlot & tococpy );
    InSlot&             operator=( const InSlot & tocopy );
    ~InSlot();

    // STREAMING

    const InSlot<T>&    operator>>( T & ) const;
    operator const T & () const;

    // GETTING INFOS

    OutSlot<T>*         getOutSlotPtr( void ) const;

    const QString       getName( void ) const;
    quint32             getId( void ) const;
    const IEffectNode*  getFather( void ) const;
    bool                isItAnInternalSlot( void ) const;

    // SETTING INFOS

    void                setId( quint32 id );
    void                setName( const QString & name );
    void                setFather( EffectNode* father );
    void                setAsInternal( void );
    void                setScope( bool isItAnInternalSlot );

private:

    // GETTING PRIVATES INFOS

    EffectNode*         getPrivateFather( void ) const;

    // CONNECTION & DISCONNECTION

    bool                connect( OutSlot<T>& );
    bool                disconnect( void );

    void                setOutSlotPtr( OutSlot<T>* ptr);
    void                resetOutSlotPtr( void );
    void                setCurrentSharedToDefault( void );
    void                setCurrentSharedToShared( void );

private:

    mutable QReadWriteLock      m_rwl;
    static T			s_defaultValue;
    OutSlot<T>*			m_OutSlotPtr;
    T				m_shared;
    T*				m_currentShared;

    quint32                     m_id;
    QString                     m_name;
    EffectNode*                 m_father;
    bool                        m_isItAnInternalSlot;

    friend class                EffectNode;
};

template<typename T>
T			InSlot<T>::s_defaultValue = 0;

/////////////////////////
//// PUBLICS METHODS ////
/////////////////////////

// CTOR & DTOR

template<typename T>
InSlot<T>::InSlot( void ) : m_rwl( QReadWriteLock::Recursive ),
                      m_id( 0 ),
                      m_name( "" ),
                      m_father( NULL ),
                      m_isItAnInternalSlot( false )
{
    resetOutSlotPtr();
    setCurrentSharedToDefault();
}

template<typename T>
InSlot<T>::InSlot( const InSlot & tocopy ) : m_rwl( QReadWriteLock::Recursive ),
                                             m_id( 0 ),
                                             m_name( "" ),
                                             m_father( NULL ),
                                             m_isItAnInternalSlot( false )
{
    resetOutSlotPtr();
    setCurrentSharedToDefault();
}

template<typename T>
InSlot<T>&
InSlot<T>::operator=( const InSlot & tocopy )
{
    QWriteLocker         wl( &m_rwl );
    m_id =  0;
    m_name =  "";
    m_father = NULL;
    m_isItAnInternalSlot = false;
    resetOutSlotPtr();
    setCurrentSharedToDefault();
    return *this;
}

template<typename T>
InSlot<T>::~InSlot()
{
    if ( m_OutSlotPtr != NULL)
        disconnect();
}

// READING METHODS

template<typename T>
const InSlot<T>&
InSlot<T>::operator>>( T& val ) const
{
    QReadLocker         rl( &m_rwl );
    val = (*m_currentShared);
    return *this;
}

template<typename T>
InSlot<T>::operator const T & () const
{
    QReadLocker         rl( &m_rwl );
    return *m_currentShared;
}

// GETTING INFOS

template<typename T>
OutSlot<T>*
InSlot<T>::getOutSlotPtr( void ) const
{
    QReadLocker         rl( &m_rwl );
    return m_OutSlotPtr;
}

template<typename T>
quint32
InSlot<T>::getId( void ) const
{
    QReadLocker         rl( &m_rwl );
    return m_id;
}

template<typename T>
const QString
InSlot<T>::getName( void ) const
{
    QReadLocker         rl( &m_rwl );
    return m_name;
}

template<typename T>
const IEffectNode *
InSlot<T>::getFather( void ) const
{
    QReadLocker         rl( &m_rwl );
    return m_father;
}

template<typename T>
bool
InSlot<T>::isItAnInternalSlot( void ) const
{
    QReadLocker         rl( &m_rwl );
    return m_isItAnInternalSlot;
}


// SETTING INFOS

template<typename T>
void
InSlot<T>::setId( quint32 id )
{
    QWriteLocker         wl( &m_rwl );
    m_id = id;
}

template<typename T>
void
InSlot<T>::setName( QString const & name )
{
    QWriteLocker         wl( &m_rwl );
    m_name = name;
}

template<typename T>
void
InSlot<T>::setFather( EffectNode* father )
{
    QWriteLocker         wl( &m_rwl );
    m_father = father;
}

template<typename T>
void
InSlot<T>::setScope( bool isItAnInternalSlot )
{
    QWriteLocker         wl( &m_rwl );
    m_isItAnInternalSlot = isItAnInternalSlot;
}

//////////////////////////
//// PRIVATES METHODS ////
//////////////////////////

// GETTING PRIVATES INFOS

template<typename T>
EffectNode*
InSlot<T>::getPrivateFather( void ) const
{
    QReadLocker         rl( &m_rwl );
    return m_father;
}

// CONNECTION METHODS

template<typename T>
bool
InSlot<T>::connect( OutSlot<T>& toconnect )
{
    QWriteLocker        wl( &m_rwl );
    if ( m_OutSlotPtr != NULL )
        return false;
    toconnect.setPipe( &m_shared );
    toconnect.setInSlotPtr( this );
    setOutSlotPtr( &toconnect );
    setCurrentSharedToShared();
    return true;
}

template<typename T>
bool
InSlot<T>::disconnect( void )
{
    QWriteLocker         wl( &m_rwl );
    if (m_OutSlotPtr == NULL)
        return false;
    m_OutSlotPtr->resetPipe();
    m_OutSlotPtr->resetInSlotPtr();
    resetOutSlotPtr();
    setCurrentSharedToDefault();
    return true;
}

// OTHERS

template<typename T>
void
InSlot<T>::setOutSlotPtr( OutSlot<T>* ptr)
{
    QWriteLocker         wl( &m_rwl );
    m_OutSlotPtr = ptr;
}

template<typename T>
void
InSlot<T>::resetOutSlotPtr( void )
{
    QWriteLocker         wl( &m_rwl );
    m_OutSlotPtr = NULL;
}

template<typename T>
void
InSlot<T>::setCurrentSharedToDefault( void )
{
    QWriteLocker         wl( &m_rwl );
    m_currentShared = &s_defaultValue;
}

template<typename T>
void
InSlot<T>::setCurrentSharedToShared( void )
{
    QWriteLocker         wl( &m_rwl );
    m_currentShared = &m_shared;
}

#endif // INSLOT_HPP_
