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

private:

  enum	OUTTYPE				// DEFINTION OF MANY OUTPUTS TYPES
    {
      GUI,				// OUTPUTS FROM GUI
      EMPTY,				// WHEN NO OUTPUTS ARE CONNECTED
      INTERPRETER,			// OUTPUT FROM INTERPRETER
      NORMAL,				// OUTPUT FROM STREAMING 
      NBTYPES
    };					// IT'S SORTED BY PRIORITY

  enum	PRIORITY
    {
      LOWER = EMPTY,
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
  unsigned int		m_nboutbytype[NBTYPES];
  static unsigned int	m_outnblimits[NBTYPES];
  OUTTYPE		m_currentShared;
};


// STATIC MEMBERS INTIALIZATION

template<typename T>
unsigned int  InSlot<T>::m_outnblimits[] = {InSlot<T>::INFINITE, InSlot<T>::INFINITE, InSlot<T>::ONLYONE,  InSlot<T>::ONLYONE} ; // USELESS BUT JUST FOR CLEANESS

////
//// Publics Methods
////

// CTOR & DTOR

template<typename T>
InSlot<T>::InSlot() : m_currentShared( EMPTY )
{
  unsigned int 	type;

  m_shared[EMPTY] = 0; // DEFAULT VALUE SET HERE FOR FORWARD THIS WHEN THERE AREN'T OUTSLOTS CONNECTED

  for (type = static_cast<unsigned int>( InSlot<T>::EMPTY );
       type < static_cast<unsigned int>( InSlot<T>::NBTYPES );
       ++type)
    m_nboutbytype[type] = 0;
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

////
//// Privates Methods
////

// CONNECTION METHODS


// Un OutSlot a le droit de se connecter ssi :
// -il ne depasse pas le nombre maximal de son type
// -il n'est pas deja connecte --> implicite car la verification a ete faite dans l'appel a la method connect de l'OutSlot
// Une fois qu'on a verifie qu'il avait ce droit, il faut modifier le m_currentShared en fonction

template<typename T>
bool	InSlot<T>::connect( OutSlot<T>& toconnect )
{
  OUTTYPE	type;

  type = toconnect.getType();
  if ( m_outnblimits[type] != InSlot<T>::INFINITE)
    if ( m_nboutbytype[type] >= m_outnblimits[type] )
      {
	std::cout << "m_outnblimits[type] = " << m_outnblimits[type] << std::endl;
	std::cout << "m_nboutbytype[type] = " << m_nboutbytype[type] << std::endl;
	return ( false );
      }
  ++(m_nboutbytype[type]);

  toconnect.setPipe( &m_shared[type] );
  toconnect.setInSlotPtr( this );

  switchCurrentShared();
  return ( true );
}

// Un OutSlot a le droit de se deconnecter ssi :
// -il est connecte sur cet InSlot --> implicite car la methode est appele depuis le OutSlot qui a sauvegarde
// le pointeur sur InSlot duquel il veut se deconnecter
// Une fois qu'on a verifie qu'il avait ce droit, il faut modifier le m_currentShared en fonction

template<typename T>
bool	InSlot<T>::disconnect( OutSlot<T>& todisconnect )
{
  OUTTYPE	type;

  type = todisconnect.getType();
  --(m_nboutbytype[type]);

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
      if ( m_nboutbytype[priority] )
	break;
    }
  m_currentShared = static_cast< typename InSlot<T>::OUTTYPE >( priority );  
  return ;
}

#endif // INSLOT_HPP_
