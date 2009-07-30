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

template<typename T> class OutSlot;

template<typename T>
class	InSlot
{

private:

  enum	OUTTYPE				// DEFINTION OF MANY OUTPUTS TYPES
    {
      EMPTY,				// WHEN NO OUTPUTS ARE CONNECTED
      GUI,				// OUTPUTS FROM GUI
      INTERPRETER,			// OUTPUT FROM INTERPRETER
      NORMAL,				// OUTPUT FROM STREAMING 
      NBTYPES
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

  void	connect( OutSlot<T>& );
  void	disconnect( OutSlot<T>& );

private:
  T			m_shared[NBTYPES];
  unsigned int		m_nboutbytype[NBTYPES];
  static unsigned int	m_outnblimits[NBTYPES];
  OUTTYPE		m_currentShared;
};


// STATIC MEMBERS INTIALIZATION

template<T>
unsigned int  InSlot<T>::m_outnblimits[InSlot<T>::EMPTY] = InSlot<T>::INFINITE; // USELESS BUT JUST FOR CLEANESS

template<T>
unsigned int  InSlot<T>::m_outnblimits[InSlot<T>::GUI] = InSlot<T>::INFINITE;

template<T>
unsigned int  InSlot<T>::m_outnblimits[InSlot<T>::INTERPRETER] = InSlot<T>::ONLYONE;

template<T>
unsigned int  InSlot<T>::m_outnblimits[InSlot<T>::NORMAL] = InSlot<T>::ONLYONE;

////
//// Publics Methods
////

// CTOR & DTOR

template<typename T>
InSlot<T>::InSlot() m_currentShared( EMPTY )
{
  m_shared[EMPTY] = 0;
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
  if ( m_nbshared[type] >= m_outnblimits[type] )
    return ( false );
  ++(m_nbshared[type]);

  toconnect.setPipe( &m_shared[type] );
  toconnect.setInSlotPtr( this );

  connectChooser()
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
  --(m_nbshared[type]);

  todisconnect.resetPipe();
  todisconnect.resetInSlotPtr();

  connectChooser()
  return ( true );
}

#endif // INSLOT_HPP_
