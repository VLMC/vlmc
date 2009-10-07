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

#include "InSlot.hpp"
#include <QtDebug>

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
  void		setType( typename InSlot<T>::OUTTYPE type );



private:

  // OTHERS

  typename InSlot<T>::OUTTYPE		getType( void ) const;
  void			setPipe( T* shared );
  void			resetPipe( void );
  void			setInSlotPtr( InSlot<T>* );
  void			resetInSlotPtr( void );

private:

  typename InSlot<T>::OUTTYPE	m_type;
  InSlot<T>*		m_connectedTo;
  T			m_junk;
  T*			m_pipe;
};

/////////////////////////
//// PUBLICS METHODS ////
/////////////////////////

// CTOR & DTOR

template<typename T>
OutSlot<T>::OutSlot() : m_type( InSlot<T>::NORMAL ), m_connectedTo( NULL ), m_pipe( &m_junk )
{
}

template<typename T>
OutSlot<T>::OutSlot(OutSlot<T> const & tocopy) : m_type( tocopy.m_type ),
						 m_connectedTo( NULL ),
						 m_pipe( &m_junk )
{
}

template<typename T>
OutSlot<T>&	OutSlot<T>::operator=(OutSlot<T> const & tocopy)
{
  m_type = tocopy.m_type;
  m_connectedTo = NULL;
  m_pipe = &m_junk;
}

template<typename T>
OutSlot<T>::~OutSlot()
{
}

// WRITING METHODS

template<typename T>
OutSlot<T>&	OutSlot<T>::operator=( T const & val )
{
  (*m_pipe) = val;
  return ( *this );
}

template<typename T>
OutSlot<T>&	OutSlot<T>::operator<<( T const & val )
{
  (*m_pipe) = val;
  return (*this);
}

// CONNECTION METHODS

template<typename T>
bool	OutSlot<T>::connect( InSlot<T>& toconnect )
{
  if ( m_connectedTo != NULL )
      return ( false );
  if ( toconnect.connect( (*this) ) == false)
    {
      return ( false );
    }
  return ( true );
}

template<typename T>
bool	OutSlot<T>::disconnect( void )
{
  if ( m_connectedTo == NULL)
      return ( false );
  if ( m_connectedTo->disconnect( (*this) ) == false)
      return ( false );
  return ( true );
}

template<typename T>
void	OutSlot<T>::setType( typename InSlot<T>::OUTTYPE type )
{
  m_type = type;
  return ;
}

//////////////////////////
//// PRIVATES METHODS ////
//////////////////////////

// OTHERS

template<typename T>
typename InSlot<T>::OUTTYPE	OutSlot<T>::getType( void ) const
{
  return ( m_type );
}

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
  m_connectedTo = ptr;
  return ;
}

template<typename T>
void	OutSlot<T>::resetInSlotPtr( void )
{
  m_connectedTo = NULL;
  return ;
}

#endif // OUTSLOT_HPP_


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

#include <cstdlib>

template<typename T> class OutSlot;

template<typename T>
class	InSlot
{
  friend class OutSlot<T>;

public:

  // CTOR & DTOR

  InSlot();
  InSlot(InSlot const & tocopy);
  InSlot&	operator=(InSlot const & tocopy);
  ~InSlot();

  // STREAMING

  InSlot<T>&	operator>>( T & );
                operator T const & () const;

  // GETTING INFOS

  
private:

  // CONNECTION & DISCONNECTION

  bool	connect( OutSlot<T>& );
  bool	disconnect( OutSlot<T>& );

private:
  static T			m_defaultValue;
  T				m_shared;		
  T*				m_currentShared;
};

/////////////////////////
//// PUBLICS METHODS ////
/////////////////////////

// CTOR & DTOR

template<typename T>
InSlot<T>::InSlot() : m_currentShared( &m_defaultValue )
{
}

template<typename T>
InSlot<T>::~InSlot()
{
}

// READING METHODS

template<typename T>
InSlot<T>&	InSlot<T>::operator>>( T& val )
{
  val = m_shared[m_currentShared];
  return ( (*this) );
}

template<typename T>
InSlot<T>::operator T const & () const
{
  return ( m_shared[m_currentShared] );
}

//////////////////////////
//// PRIVATES METHODS ////
//////////////////////////

// CONNECTION METHODS

// An OutSlot can be connected only if :
// -The number of OutSlot of the same type isn't at the maximum
// -The OutSlot isn't already connected --> verified in the method void OutSlot<T>::connect( InSlot<T>& T )
// So, the OutSlot can be connected and the m_currentShared must be updated

template<typename T>
bool	InSlot<T>::connect( OutSlot<T>& toconnect )
{
  OUTTYPE	type;

  type = toconnect.getType();
  if ( m_outNbLimits[type] != InSlot<T>::INFINITE)
    if ( m_nbOutByType[type] >= m_outNbLimits[type] )
	return ( false );
  ++(m_nbOutByType[type]);

  toconnect.setPipe( &m_shared[type] );
  toconnect.setInSlotPtr( this );

  switchCurrentShared();
  return ( true );
}

// An OutSlot can be disconnected only if :
// -The OutSlot is connected on this InSlot --> this is implicit because this method is call only by
// bool OutSlot<T>::disconnect( ), and this method call the first method with the InSlot<T> pointer saved at connection
// So, the OutSlot can be connected and the m_currentShared must be updated

template<typename T>
bool	InSlot<T>::disconnect( OutSlot<T>& todisconnect )
{
  OUTTYPE	type;

  type = todisconnect.getType();
  --(m_nbOutByType[type]);

  todisconnect.resetPipe();
  todisconnect.resetInSlotPtr();

  switchCurrentShared();
  return ( true );
}

template<typename T>
void	InSlot<T>::switchCurrentShared( void )
{
  unsigned int	priority;

  for ( priority = InSlot<T>::HIGHER; priority > InSlot<T>::LOWER; --priority )
    {
      if ( m_nbOutByType[priority] )
	break;
    }
  m_currentShared = static_cast< typename InSlot<T>::OUTTYPE >( priority );
  m_shared[m_currentShared] = m_shared[DEFAULT];
  return ;
}

#endif // INSLOT_HPP_

int main(void)
{
  return (0);
}
