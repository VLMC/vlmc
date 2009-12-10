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

class EffectNode;
class IEffectNode;

template<typename T> class OutSlot;


template<typename T>
class	InSlot
{
  friend class OutSlot<T>;

public:

  // CTOR & DTOR

  InSlot();
  InSlot( InSlot const & );
  InSlot&	operator=( InSlot const & );
  ~InSlot();

  // STREAMING

  InSlot<T> const&	operator>>( T & ) const;
			operator T const & () const;

    // GETTING INFOS

    OutSlot<T>*		getOutSlotPtr( void ) const;

    QString const       getName( void ) const;
    quint32             getId( void ) const;
    IEffectNode*        getFather( void ) const;

    // SETTING INFOS

    void                setId( quint32 id );
    void                setName( QString const & name );
    void                setFather( EffectNode* father );

    // BINDING METHODS

    bool     bindOn( InSlot<T>& dest );
    T**      getBinded( InSlot<T>& src );
    bool     unbind( void );
    void     getUnbinded( void );

private:

    // GETTING PRIVATES INFOS

    EffectNode*         getPrivateFather( void ) const;

  // CONNECTION & DISCONNECTION

  bool	connect( OutSlot<T>& );
  bool	disconnect( void );

  void	setOutSlotPtr( OutSlot<T>* ptr);
  void	resetOutSlotPtr( void );
  void	setCurrentSharedToDefault( void );
  void	setCurrentSharedToShared( void );

private:

    static T			m_defaultValue;
    OutSlot<T>*			m_OutSlotPtr;
    T				m_shared;
    T*				m_currentShared;
    T**                         m_metaCurrentShared;

    OutSlot<T>*                 m_bindSrc;
    OutSlot<T>*                 m_bindDst;
    quint32                     m_id;
    QString                     m_name;
    EffectNode*                 m_father;
};

template<typename T>
T			InSlot<T>::m_defaultValue = 0;

/////////////////////////
//// PUBLICS METHODS ////
/////////////////////////

// CTOR & DTOR

template<typename T>
InSlot<T>::InSlot() : m_metaCurrentShared( &m_currentShared ), m_bindSrc( NULL ), m_bindDst( NULL ), m_id( 0 ), m_name( "" ), m_father( NULL )
{
    resetOutSlotPtr();
    setCurrentSharedToDefault();
}

template<typename T>
InSlot<T>::InSlot(InSlot const &) : m_metaCurrentShared( &m_currentShared ), m_bindSrc( NULL ), m_bindDst( NULL ), m_id( 0 ), m_name( "" ), m_father( NULL )
{
    resetOutSlotPtr();
    setCurrentSharedToDefault();
}

template<typename T>
InSlot<T>&		InSlot<T>::operator=(InSlot const &)
{
    m_bindSrc = NULL;
    m_bindDst = NULL;
    m_id =  0;
    m_name =  "";
    m_metaCurrentShared = &m_currentShared;
    m_father = NULL;
    resetOutSlotPtr();
    setCurrentSharedToDefault();
}

template<typename T>
InSlot<T>::~InSlot()
{
  if ( m_OutSlotPtr != NULL)
    disconnect();
}

// READING METHODS

template<typename T>
InSlot<T> const &	InSlot<T>::operator>>( T& val ) const
{
    if ( m_bindSrc == NULL )
        val = ( **m_metaCurrentShared );
    return ( (*this) );
}

template<typename T>
InSlot<T>::operator T const & () const
{
    if ( m_bindSrc == NULL )
        return ( **m_metaCurrentShared );
    return m_defaultValue;
}

// GETTING INFOS

template<typename T>
OutSlot<T>*	InSlot<T>::getOutSlotPtr( void ) const
{
  return ( m_OutSlotPtr );
}

template<typename T>
quint32                InSlot<T>::getId( void ) const
{
    return ( m_id );
}

template<typename T>
QString const          InSlot<T>::getName( void ) const
{
    return ( m_name );
}

template<typename T>
IEffectNode*          InSlot<T>::getFather( void ) const
{
    return ( m_father );
}

// SETTING INFOS

template<typename T>
void                InSlot<T>::setId( quint32 id )
{
    m_id = id;
    return ;
}

template<typename T>
void                InSlot<T>::setName( QString const & name )
{
    m_name = name;
    return ;
}

template<typename T>
void                InSlot<T>::setFather( EffectNode* father )
{
    m_father = father;
    return ;
}

//////////////////////////
//// PRIVATES METHODS ////
//////////////////////////

// GETTING PRIVATES INFOS

template<typename T>
EffectNode*          InSlot<T>::getPrivateFather( void ) const
{
    return ( m_father );
}

// CONNECTION METHODS

template<typename T>
bool	InSlot<T>::connect( OutSlot<T>& toconnect )
{
    if ( m_bindDst != NULL )
        return ( false );
  if ( m_OutSlotPtr != NULL )
    return ( false );
  toconnect.setPipe( &m_shared );
  toconnect.setInSlotPtr( this );
  setOutSlotPtr( &toconnect );
  setCurrentSharedToShared();
  return ( true );
}

template<typename T>
bool	InSlot<T>::disconnect( void )
{
    if ( m_bindDst != NULL )
        return ( false );
  if (m_OutSlotPtr == NULL)
    return ( false );
  m_OutSlotPtr->resetPipe();
  m_OutSlotPtr->resetInSlotPtr();
  resetOutSlotPtr();
  setCurrentSharedToDefault();
  return ( true );
}

// BINDIND METHODS

template<typename T>
bool	InSlot<T>::bindOn( InSlot<T>& dest )
{
    if ( ( m_OutSlotPtr != NULL ) || ( m_bindDst != NULL ) )
        return ( false );
    if ( ( m_metaCurrentShared = getBinded( *this ) ) == NULL )
        return ( false );
    m_bindDst = &dest;
    return ( true );
}

template<typename T>
T**     InSlot<T>::getBinded( InSlot<T>& src )
{
    if ( m_bindSrc != NULL )
        return ( NULL );
    m_bindSrc = &src;
    return ( m_metaCurrentShared );
}

template<typename T>
bool	InSlot<T>::unbind( void )
{
    if ( ( m_OutSlotPtr != NULL ) || ( m_bindDst == NULL ) )
        return ( false );
    m_bindDst->m_bindSrc = NULL;
    m_bindDst = NULL;
    m_metaCurrentShared = &m_currentShared;
    return ( true );
}

template<typename T>
void     InSlot<T>::getUnbinded( void )
{
    m_bindSrc = NULL;
    return ;
}

// OTHERS

template<typename T>
void	InSlot<T>::setOutSlotPtr( OutSlot<T>* ptr)
{
  m_OutSlotPtr = ptr;
  return ;
}

template<typename T>
void	InSlot<T>::resetOutSlotPtr( void )
{
  m_OutSlotPtr = NULL;
  return ;
}

template<typename T>
void	InSlot<T>::setCurrentSharedToDefault( void )
{
  m_currentShared = &m_defaultValue;
  return ;
}

template<typename T>
void	InSlot<T>::setCurrentSharedToShared( void )
{
  m_currentShared = &m_shared;
  return ;
}

#endif // INSLOT_HPP_
