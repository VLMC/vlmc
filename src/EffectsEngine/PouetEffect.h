#ifndef POUETEFFECT_H_
#define POUETEFFECT_H_

#include "GenericEffect.h"

class	PouetEffect : public GenericEffect
{
 public:

  // CTOR & DTOR

  PouetEffect();
  ~PouetEffect();

  // RENDER METHOD

  void	render( void );
  void	enable( void );
  void	disable( void );

 private:

  bool					m_enabled;
  static	quint32 const		m_nbVideoInputs = 1;
  static	quint32 const		m_nbVideoOutputs = 1;
  static	char const * 	m_videoInputsNames[m_nbVideoInputs];
  static	char const *	m_videoOutputsNames[m_nbVideoOutputs];
};

#endif // POUETEFFECT_H_
