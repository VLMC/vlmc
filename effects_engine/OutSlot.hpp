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

  OutSlot&	operator<<(T const &);
  OutSlot&	operator=(T const &);

  // CONNECTION & DISCONNECTION

  bool		connect( InSlot<T>& );
  bool		disconnect( void );


private:

  void		setPipe(T*);
private:
  
  InSlot<T>*		m_connectedto;
  T*			m_pipe;
  T			m_junk;

};

template<typename T>
OutSlot<T>::OutSlot() : m_connectedto(NULL), m_pipe(&m_junk)
{
}

template<typename T>
OutSlot<T>::~OutSlot()
{
}

template<typename T>
OutSlot<T>&	OutSlot<T>::operator=(T const & val)
{
  (*m_pipe) = val;
  return (*this);
}

template<typename T>
OutSlot<T>&	OutSlot<T>::operator<<(T const & val)
{
  (*m_pipe) = val;
  return (*this);
}

template<typename T>
void	OutSlot<T>::connect(InSlot<T>& toconnect)
{
  m_connectedto = &toconnect;
  m_connectedto->connect((*this));
  return ;
}

template<typename T>
void	OutSlot<T>::disconnect(void)
{
  m_connectedto->disconnect( (*this) );
  m_connectedto = NULL;
  m_pipe = &m_junk;
  return ;
}

#endif // OUTSLOT_HPP_
