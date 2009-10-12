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

#include <cstdlib>

//#include "InSlot.hpp"
template<typename T> class InSlot;

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

  // GETTING INFOS

  InSlot<T>*	getInSlotPtr( void );
private:

  // OTHERS

  void			setPipe( T* shared );
  void			resetPipe( void );
  void			setInSlotPtr( InSlot<T>* );
  void			resetInSlotPtr( void );

private:

  InSlot<T>*		m_InSlotPtr;
  T			m_junk;
  T*			m_pipe;
};

/////////////////////////
//// PUBLICS METHODS ////
/////////////////////////

// CTOR & DTOR

template<typename T>
OutSlot<T>::OutSlot()
{
  resetInSlotPtr();
  resetPipe();
}

template<typename T>
OutSlot<T>::OutSlot(OutSlot<T> const & tocopy)
{
  resetInSlotPtr();
  resetPipe();
}

template<typename T>
OutSlot<T>&	OutSlot<T>::operator=(OutSlot<T> const & tocopy)
{
  resetInSlotPtr();
  resetPipe();
}

template<typename T>
OutSlot<T>::~OutSlot()
{
  if ( m_InSlotPtr != NULL )
    m_InSlotPtr->disconnect();
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
  if ( m_InSlotPtr != NULL )
      return ( false );
  if ( toconnect.connect( (*this) ) == false)
    return ( false );
  return ( true );
}

template<typename T>
bool	OutSlot<T>::disconnect( void )
{
  if ( m_InSlotPtr == NULL)
      return ( false );
  m_InSlotPtr->disconnect();
  return ( true );
}

// GETTING INFOS

template<typename T>
InSlot<T>*	OutSlot<T>::getInSlotPtr( void )
{
  return ( m_InSlotPtr );
}

//////////////////////////
//// PRIVATES METHODS ////
//////////////////////////

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

//#include <cstdlib>


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

  InSlot<T> const&	operator>>( T & ) const;
			operator T const & () const;

  // GETTING INFOS

  OutSlot<T>*		getOutSlotPtr( void );

private:

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
};

template<typename T>
T			InSlot<T>::m_defaultValue = 0;

/////////////////////////
//// PUBLICS METHODS ////
/////////////////////////

// CTOR & DTOR

template<typename T>
InSlot<T>::InSlot()
{
  resetOutSlotPtr();
  setCurrentSharedToDefault();
}

template<typename T>
InSlot<T>::InSlot(InSlot const & tocopy)
{
  resetOutSlotPtr();
  setCurrentSharedToDefault();  
}

template<typename T>
InSlot<T>&		InSlot<T>::operator=(InSlot const & tocopy)
{
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
  val = *m_currentShared;
  return ( (*this) );
}

template<typename T>
InSlot<T>::operator T const & () const
{
  return ( *m_currentShared );
}

// GETTING INFOS

template<typename T>
OutSlot<T>*	InSlot<T>::getOutSlotPtr( void )
{
  return ( m_OutSlotPtr );
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
  if ( m_OutSlotPtr != NULL )
    return ( false );
  toconnect.setPipe( &m_shared );
  toconnect.setInSlotPtr( this );
  setOutSlotPtr( &toconnect );
  setCurrentSharedToShared();
  return ( true );
}

// An OutSlot can be disconnected only if :
// -The OutSlot is connected on this InSlot --> this is implicit because this method is call only by
// bool OutSlot<T>::disconnect( ), and this method call the first method with the InSlot<T> pointer saved at connection
// So, the OutSlot can be connected and the m_currentShared must be updated

template<typename T>
bool	InSlot<T>::disconnect( void )
{
  if (m_OutSlotPtr == NULL)
    return ( false );
  m_OutSlotPtr->resetPipe();
  m_OutSlotPtr->resetInSlotPtr();
  resetOutSlotPtr();
  setCurrentSharedToDefault();
  return ( true );
}

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

#include <iostream>

int main(void)
{
  OutSlot<int>	out;
  InSlot<int>	in;

  out << 42;
  std::cout << in << std::endl;

  if ( out.connect(in) == true )
    std::cout << "out connected to in" << std::endl;
  else
    std::cout << "out failed to connect to in" << std::endl;
  out << 42;
  std::cout << in << std::endl;
  if ( out.disconnect() == true )
    std::cout << "out disconnected to in" << std::endl;
  else
    std::cout << "out failed to disconnect to in" << std::endl;
  std::cout << in << std::endl;
  return (0);
}
