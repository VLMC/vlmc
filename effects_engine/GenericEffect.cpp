#include "GenericEffect.h"

GenericEffect::GenericEffect( char const * videoinputs[], quint const nbvideoinputs,
			      char const * videooutputs[], quint const nbvideooutputs)
{
  quint	i;

  for (i = 0; i < nbvideoinputs; ++i)
    m_videoInputs[videoinputs[i]];
  for (i = 0; i < nbvideooutputs; ++i)
    m_videoOutputs[videooutputs[i]];      
}

GenericEffect::~GenericEffect()
{
}

