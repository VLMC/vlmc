#include "PouetEffect.h"
#include <iostream>

char const * 	PouetEffect::m_videoInputsNames[] = {"in"};
char const *	PouetEffect::m_videoOutputsNames[] = {"out"};
 
PouetEffect::PouetEffect() : GenericEffect(
					   PouetEffect::m_videoInputsNames, PouetEffect::m_nbVideoInputs,
					   PouetEffect::m_videoOutputsNames, PouetEffect::m_nbVideoOutputs
					   )
{
};

PouetEffect::~PouetEffect()
{
}

void	PouetEffect::render( void )
{
  quint32		i;
  LightVideoFrame	lol;
  LightVideoFrame	lol2;
  VideoFrame		tmp;
  quint32		size;

  ( m_videoInputs["in"] ) >> lol;
  tmp = lol;

  for ( i = 0; i < tmp.nbpixels; ++i )
    tmp.rvf.pixel[i].Red = 'W';


  lol2 = tmp;
  (m_videoOutputs["out"]) << lol2;
  return ;
}
