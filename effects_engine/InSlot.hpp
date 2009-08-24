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

public:

  enum	OUTTYPE				// DEFINTION OF MANY OUTPUTS TYPES
    {
      GUI,				// OUTPUTS FROM GUI
      DEFAULT,				// WHEN NO OUTPUTS ARE CONNECTED
      NORMAL,				// OUTPUT FROM STREAMING 
      NBTYPES
    };					// IT'S SORTED BY PRIORITY

private:

  enum	PRIORITY
    {
      LOWER = DEFAULT,
      HIGHER = ( NBTYPES - 1 )
    };

  enum	CONSTANTS
    {
      INFINITE = 0,
      ONLYONE = 1
    };

  friend class OutSlot<T>;

public:

  // CTOR & DTOR

  InSlot();
  ~InSlot();

  // STREAMING

  InSlot<T>&	operator>>( T & );
                operator T const & () const;

private:

  // CONNECTION & DISCONNECTION

  bool	connect( OutSlot<T>& );
  bool	disconnect( OutSlot<T>& );

  // OTHERS

  void	switchCurrentShared( void );

private:
  T			m_shared[NBTYPES];
  unsigned int		m_nbOutByType[NBTYPES];
  static unsigned int	m_outNbLimits[NBTYPES];
  OUTTYPE		m_currentShared;
};


// STATIC MEMBERS INTIALIZATION

template<typename T>
unsigned int  InSlot<T>::m_outNbLimits[] = {InSlot<T>::INFINITE, InSlot<T>::INFINITE,  InSlot<T>::ONLYONE} ;

/////////////////////////
//// PUBLICS METHODS ////
/////////////////////////

// CTOR & DTOR

template<typename T>
InSlot<T>::InSlot() : m_currentShared( DEFAULT )
{
  unsigned int 	type;

//   m_shared[DEFAULT] = 0; // DEFAULT VALUE SET HERE FOR FORWARD THIS WHEN THERE AREN'T OUTSLOTS CONNECTED

  for (type = static_cast<unsigned int>( InSlot<T>::DEFAULT );
       type < static_cast<unsigned int>( InSlot<T>::NBTYPES );
       ++type)
    m_nbOutByType[type] = 0;
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
