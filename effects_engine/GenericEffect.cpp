#include "GenericEffect.h"

GenericEffect::GenericEffect( char const * videoinputs[], quint32 const nbvideoinputs,
			      char const * videooutputs[], quint32 const nbvideooutputs)
{
  quint32	i;

  for (i = 0; i < nbvideoinputs; ++i)
    m_videoInputs[videoinputs[i]];
  for (i = 0; i < nbvideooutputs; ++i)
    m_videoOutputs[videooutputs[i]];      
}

GenericEffect::~GenericEffect()
{
}

