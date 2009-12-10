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

#include "InSlot.hpp"

class IEffectNode;
class EffectNode;

template<typename T>
class   OutSlot
{

  friend class InSlot<T>;

public:

  // CTOR & DTOR

  OutSlot();
  OutSlot(OutSlot const &);
  OutSlot&	operator=(OutSlot const &);
  ~OutSlot();

  // STREAMING

  OutSlot&	operator<<( T const & );
  OutSlot&	operator=( T const & );

  // CONNECTION & DISCONNECTION

  bool		connect( InSlot<T>& );
  bool		disconnect( void );

    // BINDING METHODS

    bool	bindOn( OutSlot<T>& dest );
    bool	unbind( void );
  // GETTING INFOS

    InSlot<T>*	getInSlotPtr( void ) const;

    QString const       getName( void ) const;
    quint32             getId( void ) const;
    IEffectNode*         getFather( void ) const;

 // SRTTING INFOS

    void          setId( quint32 id );
    void          setName( QString const & name );
    void          setFather( EffectNode* father );

private:

    void        getUnbinded( void );
    T**         getBinded( OutSlot<T>& src );

    // GETTING PRIVATES INFOS

    EffectNode*         getPrivateFather( void ) const;

  // OTHERS

  void			setPipe( T* shared );
  void			resetPipe( void );
  void			setInSlotPtr( InSlot<T>* );
  void			resetInSlotPtr( void );

private:

    OutSlot<T>*         m_bindDst;
    OutSlot<T>*         m_bindSrc;
  InSlot<T>*		m_InSlotPtr;
  T			m_junk;
  T*			m_pipe;
  T**                   m_metaPipe;

    quint32             m_id;
    QString             m_name;
    EffectNode*        m_father;
};

/////////////////////////
//// PUBLICS METHODS ////
/////////////////////////

// CTOR & DTOR

template<typename T>
OutSlot<T>::OutSlot() : m_bindDst( NULL ), m_bindSrc( NULL ), m_metaPipe( &m_pipe ), m_id( 0 ), m_name( "" ), m_father( NULL )
{
    resetInSlotPtr();
    resetPipe();
}

template<typename T>
OutSlot<T>::OutSlot(OutSlot<T> const & tocopy) : m_bindDst( NULL ), m_bindSrc( NULL ), m_metaPipe( &m_pipe ), m_id( 0 ), m_name( "" ), m_father( NULL )
{
  resetInSlotPtr();
  resetPipe();
}

template<typename T>
OutSlot<T>&	OutSlot<T>::operator=(OutSlot<T> const & tocopy)
{
    m_bindDst = NULL;
    m_bindSrc = NULL;
    m_id = 0;
    m_metaPipe = &m_pipe;
    m_name = "";
    m_father = NULL;
    resetInSlotPtr();
    resetPipe();
}

template<typename T>
OutSlot<T>::~OutSlot()
{
  if ( m_InSlotPtr != NULL )
    m_InSlotPtr->disconnect();

  if ( m_bindDst != NULL )
      unbind();
  else if ( m_bindSrc != NULL)
      m_bindSrc->unbind();

}

// WRITING METHODS

template<typename T>
OutSlot<T>&	OutSlot<T>::operator=( T const & val )
{
    if ( m_bindSrc == NULL )
        (**m_metaPipe) = val;
    return ( *this );
}

template<typename T>
OutSlot<T>&	OutSlot<T>::operator<<( T const & val )
{
    if ( m_bindSrc == NULL )
        (**m_metaPipe) = val;
    return (*this);
}

// CONNECTION METHODS

template<typename T>
bool	OutSlot<T>::connect( InSlot<T>& toconnect )
{
    if ( m_bindDst != NULL )
        return ( false );
  if ( m_InSlotPtr != NULL )
      return ( false );
  if ( toconnect.connect( (*this) ) == false)
    return ( false );
  return ( true );
}

template<typename T>
bool	OutSlot<T>::disconnect( void )
{
    if ( m_bindDst != NULL )
        return ( false );
  if ( m_InSlotPtr == NULL)
      return ( false );
  m_InSlotPtr->disconnect();
  return ( true );
}

// BINDING METHODS

template<typename T>
bool	OutSlot<T>::bindOn( OutSlot<T>& dest )
{
    if ( ( m_InSlotPtr != NULL ) || ( m_bindDst != NULL ) )
        return ( false );
    if ( ( m_metaPipe = getBinded( *this ) ) == NULL )
        return ( false );
    m_bindDst = &dest;
    return ( true );
}

template<typename T>
T**     OutSlot<T>::getBinded( OutSlot<T>& src )
{
    if ( m_bindSrc != NULL )
        return ( NULL );
    m_bindSrc = &src;
    return ( m_metaPipe );
}

template<typename T>
bool	OutSlot<T>::unbind( void )
{
    if ( ( m_InSlotPtr != NULL ) || ( m_bindDst == NULL ) )
        return ( false );
    m_bindDst->m_bindSrc = NULL;
    m_bindDst = NULL;
    m_metaPipe = &m_pipe;
    return ( true );
}

template<typename T>
void     OutSlot<T>::getUnbinded( void )
{
    m_bindSrc = NULL;
    return ;
}

// GETTING INFOS

template<typename T>
InSlot<T>*	OutSlot<T>::getInSlotPtr( void ) const
{
  return ( m_InSlotPtr );
}

template<typename T>
quint32                OutSlot<T>::getId( void ) const
{
    return ( m_id );
}

template<typename T>
QString const          OutSlot<T>::getName( void ) const
{
    return ( m_name );
}

template<typename T>
IEffectNode*         OutSlot<T>::getFather( void ) const
{
    return ( m_father );
}

// SETTING INFOS

template<typename T>
void                OutSlot<T>::setId( quint32 id )
{
    m_id = id;
    return ;
}

template<typename T>
void                OutSlot<T>::setName( QString const & name )
{
    m_name = name;
    return ;
}

template<typename T>
void         OutSlot<T>::setFather( EffectNode* father )
{
    m_father = father;
    return ;
}

//////////////////////////
//// PRIVATES METHODS ////
//////////////////////////

// GETTING PRIVATES INFOS

template<typename T>
EffectNode*         OutSlot<T>::getPrivateFather( void ) const
{
    return ( m_father );
}

// OTHERS

template<typename T>
void	OutSlot<T>::setPipe( T* shared )
{
  m_pipe = shared;
  return ;
}

template<typename T>
void	OutSlot<T>::resetPipe( void )
{
  m_pipe = &m_junk;
  return ;
}

template<typename T>
void	OutSlot<T>::setInSlotPtr( InSlot<T>* ptr )
{
  m_InSlotPtr = ptr;
  return ;
}

template<typename T>
void	OutSlot<T>::resetInSlotPtr( void )
{
  m_InSlotPtr = NULL;
  return ;
}

#endif // OUTSLOT_HPP_
