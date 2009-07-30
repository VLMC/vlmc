#ifndef OUTSLOT_HPP_
#define OUTSLOT_HPP_

#include "InSlot.hpp"

template<typename T>
class   OutSlot
{

public:

  // CTOR & DTOR

  OutSlot();
  ~OutSlot();

  // STREAMING

  OutSlot&	operator<<( T const & );
  OutSlot&	operator=( T const & );

  // CONNECTION & DISCONNECTION

  bool		connect( InSlot<T>& );
  bool		disconnect( void );


private:

  void		setPipe( T* );
private:
  
  InSlot<T>*		m_connectedto;
  T*			m_pipe;
  T			m_junk;

};

template<typename T>
OutSlot<T>::OutSlot() : m_connectedto( NULL ), m_pipe( &m_junk )
{
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
  if ( m_connectedto != NULL )
    return ( false );
  if ( toconnect.connect( (*this) ) == false)
    return ( false );
  return ( true );
}

template<typename T>
bool	OutSlot<T>::disconnect( void )
{
  if ( m_connectedto == NULL)
    return ( false );
  if ( m_connectedto->disconnect( (*this) ) == false)
    return ( false );
  return ( true );
}

// OTHERS

void	OutSlot<T>::setPipe( T* shared )
{
  m_pipe = shared;
  return ;
}

void	OutSlot<T>::resetPipe( void )
{
  m_pipe = &m_junk;
  return ;
}
void	OutSlot<T>::setInSlotPtr( InSlot<T>* ptr )
{
  m_connectedto = ptr;
  return ;
}

void	OutSlot<T>::resetInSlotPtr( void )
{
  m_connectedto = NULL;
  return ;
}

#endif // OUTSLOT_HPP_
