#include <iostream>

#include <QtGlobal>

template<typename T>
class	CopyOnWrite
{

  struct	node
  {
    T		data;
    node*	next;
    node*	prev;
    quint32	nbref;
  };

public:

  // CTOR & DTOR

  CopyOnWrite();
  CopyOnWrite( const T& val );
  CopyOnWrite(CopyOnWrite<T> const& tocopy);
  ~CopyOnWrite();
  
  // MAIN METHODS

  CopyOnWrite<T> const & operator=(T const & tocopy);
  CopyOnWrite<T> const & operator=(CopyOnWrite<T> const & tocopy);
  operator T const & (void) const;

  // DEBUG

  void		dumpInternalPtr(void);

private:

  void		setNewInstanceForCurrent();
  void		setNewInstanceForCurrent(T const & val);
  void		tryDeleteCurrentInstance();

private:

  node*		m_current;
  static node*	m_begin;  

};

template<typename T>
typename CopyOnWrite<T>::node*	CopyOnWrite<T>::m_begin = NULL;

// CTOR & DTOR

template<typename T>
CopyOnWrite<T>::CopyOnWrite()
{
  setNewInstanceForCurrent();
}

template<typename T>
CopyOnWrite<T>::CopyOnWrite( const T& val )
{
  setNewInstanceForCurrent(val);
}

template<typename T>
CopyOnWrite<T>::CopyOnWrite(CopyOnWrite<T> const& tocopy)
{
  m_current = tocopy.m_current;
  ++(m_current->nbref); 
};
  
template<typename T>
CopyOnWrite<T>::~CopyOnWrite()
{
  tryDeleteCurrentInstance();
};
  
// MAIN METHODS

template<typename T>
CopyOnWrite<T> const & CopyOnWrite<T>::operator=(T const & tocopy)
{
  if (m_current->nbref != 1)
    {
      --(m_current->nbref);
      setNewInstanceForCurrent();
    }
  m_current->data = tocopy;
  return ( (*this) );
}

template<typename T>
CopyOnWrite<T> const & CopyOnWrite<T>::operator=(CopyOnWrite<T> const & tocopy)
{
  tryDeleteCurrentInstance();
  m_current = tocopy.m_current;
  ++(m_current->nbref);
  return ( (*this) );
}

template<typename T>
CopyOnWrite<T>::operator T const & (void) const
{
  return ( (m_current->data) );
};

template<typename T>
void	CopyOnWrite<T>::dumpInternalPtr(void)
{
  std::cout << (void*)m_current << std::endl;
  return ;
}

//
// PRIVATE METHODS
//

template<typename T>
void		CopyOnWrite<T>::setNewInstanceForCurrent()
{
  node*	newnode = new node();
    
  newnode->nbref = 1;
  newnode->prev = NULL;
  newnode->next = m_begin;
  if (m_begin)
    m_begin->prev = newnode;
  m_begin = newnode;
  m_current = m_begin;
  return ;
}

template<typename T>
void		CopyOnWrite<T>::setNewInstanceForCurrent(T const & val)
{
  node*	newnode = new node();
	
  newnode->data = val;
  newnode->nbref = 1;
  newnode->prev = NULL;
  newnode->next = m_begin;
  if (m_begin)
    m_begin->prev = newnode;
  m_begin = newnode;
  m_current = m_begin;
  return ;
}

template<typename T>
void		CopyOnWrite<T>::tryDeleteCurrentInstance()
{
  if (m_current->nbref == 1)
    {
      if (m_current->next)
	m_current->next->prev = m_current->prev;
      if (m_current->prev)
	m_current->prev->next = m_current->next;
      delete m_current;
      m_current = NULL;
    }
  else
    --(m_current->nbref);
  return ;
}

// int		main()
// {
//   CopyOnWrite<int>	cow1;
//   CopyOnWrite<int>	cow2;

//   cow1 = 42;
//   cow2 = cow1;

//   cow1.dumpInternalPtr();
//   cow2.dumpInternalPtr();

//   cow2 = 23; // copy on write

//   cow2.dumpInternalPtr();

//   std::cout << cow1 << std::endl;

//   std::cout << cow2 << std::endl;
//   return 0;
// }
