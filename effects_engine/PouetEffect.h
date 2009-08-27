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

  void	render(void);

 private:
  static	quint32 const		m_nbVideoInputs = 3;
  static	quint32 const		m_nbVideoOutputs = 3;
  static	char const * 	m_videoInputsNames[m_nbVideoInputs];//  = {"vin1", "vin2", "vin3"};
  static	char const *	m_videoOutputsNames[m_nbVideoOutputs];//  = {"vout1", "vout2", "vout3"};
};

#endif // POUETEFFECT_H_
