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
      DYN,				// OUTPUTS FROM GUI
      INTERPRET,			// OUTPUT FROM INTERPRETER
      STREAM,				// OUTPUT FROM STREAMING 
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

  InSlot<T>&	operator>>(T &);
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


template<T>
unsigned int  InSlot<T>::m_outnblimits[InSlot<T>::EMPTY] = InSlot<T>::INFINITE; // USELESS BUT JUST FOR CLEANESS

template<T>
unsigned int  InSlot<T>::m_outnblimits[InSlot<T>::DYN] = InSlot<T>::INFINITE;

template<T>
unsigned int  InSlot<T>::m_outnblimits[InSlot<T>::INTERPRET] = InSlot<T>::ONLYONE;

template<T>
unsigned int  InSlot<T>::m_outnblimits[InSlot<T>::STREAM] = InSlot<T>::ONLYONE;

template<typename T>
InSlot<T>::InSlot() m_currentShared(EMPTY)
{
  m_shared[EMPTY] = 0;
}

template<typename T>
InSlot<T>::~InSlot()
{
}

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

template<typename T>
bool	InSlot<T>::connect( OutSlot<T>& toconnect )
{
  OUTTYPE	type;

  type = toconnect.getType();
  if ( m_nbshared[type] < m_outnblimits[type] )
    {
      toconnect.setPipe( &m_shared[type] );
      ++m_nbshared[type];
      
      return ( true );
    }

  return ( false );
}

template<typename T>
void	InSlot<T>::disconnect( OutSlot<T>& todisconnect )
{
  m_shared = 0;
  return ;
}

#endif // INSLOT_HPP_
