/*****************************************************************************
 * OutSlot.hpp: OutSlot is for outing data to an InSlot for inter-effects
 * communication
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

#ifndef OUTSLOT_HPP_
#define OUTSLOT_HPP_

#include <QDebug>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>

#include "InSlot.hpp"

class IEffectNode;
class EffectNode;

template<typename T>
class   OutSlot
{

    friend class InSlot<T>;

public:

    // CTOR & DTOR

    OutSlot( void );
    OutSlot( const OutSlot & );
    OutSlot&            operator=( const OutSlot & );
    ~OutSlot();

    // STREAMING

    OutSlot&            operator<<( const T & );
    OutSlot&            operator=( const T & );

    // CONNECTION & DISCONNECTION

    bool		connect( InSlot<T>& );
    bool		disconnect( void );

    // GETTING INFOS

    InSlot<T>*          getInSlotPtr( void ) const;
    const QString       getName( void ) const;
    quint32             getId( void ) const;
    const IEffectNode * getFather( void ) const;
    bool                isItAnInternalSlot( void ) const;

    // SRTTING INFOS

    void                setId( quint32 id );
    void                setName( const QString & name );
    void                setFather( EffectNode* father );
    void                setScope( bool isItAnInternalSlot );

private:

    // GETTING PRIVATES INFOS

    EffectNode*         getPrivateFather( void ) const;

    // OTHERS

    void                setPipe( T* shared );
    void                resetPipe( void );
    void                setInSlotPtr( InSlot<T>* );
    void                resetInSlotPtr( void );

private:

    mutable QReadWriteLock      m_rwl;
    InSlot<T>*                  m_InSlotPtr;
    T                           m_junk;
    T*                          m_pipe;

    quint32                     m_id;
    QString                     m_name;
    EffectNode*                 m_father;
    bool                        m_isItAnInternalSlot;
};

/////////////////////////
//// PUBLICS METHODS ////
/////////////////////////

// CTOR & DTOR

template<typename T>
OutSlot<T>::OutSlot( void ) : m_rwl( QReadWriteLock::Recursive ),
                              m_id( 0 ),
                              m_name( "" ),
                              m_father( NULL ),
                              m_isItAnInternalSlot( false )
{
    resetInSlotPtr();
    resetPipe();
}

template<typename T>
OutSlot<T>::OutSlot( const OutSlot<T> & tocopy ) : m_rwl( QReadWriteLock::Recursive ),
                                                 m_id( 0 ),
                                                 m_name( "" ),
                                                 m_father( NULL ),
                                                 m_isItAnInternalSlot( false )
{
    resetInSlotPtr();
    resetPipe();
}

template<typename T>
OutSlot<T>&
OutSlot<T>::operator=( const OutSlot<T> & tocopy )
{
    QWriteLocker  wl( &m_rwl );

    m_id = 0;
    m_name = "";
    m_father = NULL;
    m_isItAnInternalSlot = false;
    resetInSlotPtr();
    resetPipe();
    return *this;
}

template<typename T>
OutSlot<T>::~OutSlot()
{
    if ( m_InSlotPtr != NULL )
        m_InSlotPtr->disconnect();
}

// WRITING METHODS

template<typename T>
OutSlot<T>&
OutSlot<T>::operator=( const T & val )
{
    QWriteLocker  wl( &m_rwl );
    (*m_pipe) = val;
    return *this;
}

template<typename T>
OutSlot<T>&
OutSlot<T>::operator<<( const T & val )
{
    QWriteLocker  wl( &m_rwl );
    (*m_pipe) = val;
    return *this;
}

// CONNECTION METHODS

template<typename T>
bool
OutSlot<T>::connect( InSlot<T>& toconnect )
{
    QWriteLocker  wl( &m_rwl );
    if ( m_InSlotPtr != NULL )
        return false;
    if ( toconnect.connect( (*this) ) == false)
        return false;
    return true;
}

template<typename T>
bool
OutSlot<T>::disconnect( void )
{
    QWriteLocker  wl( &m_rwl );
    if ( m_InSlotPtr == NULL)
        return false;
    m_InSlotPtr->disconnect();
    return true;
}

// GETTING INFOS

template<typename T>
InSlot<T>*
OutSlot<T>::getInSlotPtr( void ) const
{
    QReadLocker  rl( &m_rwl );
    return m_InSlotPtr;
}

template<typename T>
quint32
OutSlot<T>::getId( void ) const
{
    QReadLocker  rl( &m_rwl );
    return m_id;
}

template<typename T>
const QString
OutSlot<T>::getName( void ) const
{
    QReadLocker  rl( &m_rwl );
    return m_name;
}

template<typename T>
const IEffectNode *
OutSlot<T>::getFather( void ) const
{
    QReadLocker  rl( &m_rwl );
    return m_father;
}

template<typename T>
bool
OutSlot<T>::isItAnInternalSlot( void ) const
{
    QReadLocker  rl( &m_rwl );
    return m_isItAnInternalSlot;
}

// SETTING INFOS

template<typename T>
void
OutSlot<T>::setId( quint32 id )
{
    QWriteLocker  wl( &m_rwl );
    m_id = id;
}

template<typename T>
void
OutSlot<T>::setName( const QString & name )
{
    QWriteLocker  wl( &m_rwl );
    m_name = name;
}

template<typename T>
void
OutSlot<T>::setFather( EffectNode* father )
{
    QWriteLocker  wl( &m_rwl );
    m_father = father;
}

template<typename T>
void
OutSlot<T>::setScope( bool isItAnInternalSlot )
{
    QWriteLocker  wl( &m_rwl );
    m_isItAnInternalSlot = isItAnInternalSlot;
}

//////////////////////////
//// PRIVATES METHODS ////
//////////////////////////

// GETTING PRIVATES INFOS

template<typename T>
EffectNode*
OutSlot<T>::getPrivateFather( void ) const
{
    QReadLocker  rl( &m_rwl );
    return m_father;
}

// OTHERS

template<typename T>
void
OutSlot<T>::setPipe( T* shared )
{
    QWriteLocker  wl( &m_rwl );
    m_pipe = shared;
}

template<typename T>
void
OutSlot<T>::resetPipe( void )
{
    QWriteLocker  wl( &m_rwl );
    m_pipe = &m_junk;
}

template<typename T>
void
OutSlot<T>::setInSlotPtr( InSlot<T>* ptr )
{
    QWriteLocker  wl( &m_rwl );
    m_InSlotPtr = ptr;
}

template<typename T>
void
OutSlot<T>::resetInSlotPtr( void )
{
    QWriteLocker  wl( &m_rwl );
    m_InSlotPtr = NULL;
}

#endif // OUTSLOT_HPP_
