#ifndef MIXEREFFECT_H_
#define MIXEREFFECT_H_

#include "GenericEffect.h"

class	MixerEffect : public GenericEffect
{
public:

  // CTOR & DTOR

  MixerEffect();
  ~MixerEffect();

  // RENDER METHOD

  void	render( void );

private:

  static	quint32 const	m_nbVideoInputs = 64;
  static	quint32 const	m_nbVideoOutputs = 64;
  static	char const *	m_videoInputsNames[m_nbVideoInputs];
  static	char const *	m_videoOutputsNames[m_nbVideoOutputs];

};

#endif // MIXEREFFECT_H_
