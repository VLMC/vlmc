#include "PouetEffect.h"
#include <QtDebug>

char const * 	PouetEffect::m_videoInputsNames[] = {"in"};
char const *	PouetEffect::m_videoOutputsNames[] = {"out"};
 
PouetEffect::PouetEffect() : GenericEffect(
					   PouetEffect::m_videoInputsNames, PouetEffect::m_nbVideoInputs,
					   PouetEffect::m_videoOutputsNames, PouetEffect::m_nbVideoOutputs
					   ),
			     m_enabled( true )
{
};

PouetEffect::~PouetEffect()
{
}

void    PouetEffect::render( void )
{
  quint32		i;
  LightVideoFrame	tmp;
  
  ( m_videoInputs["in"] ) >> tmp;
  if (tmp->frame.octets != NULL)
  {
    if (m_enabled)
      for ( i = 0; i < tmp->nbpixels; ++i )
	{
	  tmp->frame.pixels[i].Red = 0;
	  tmp->frame.pixels[i].Blue = 0;
	}
    (m_videoOutputs["out"]) << tmp;
  }
  return ;
}

void	PouetEffect::enable( void )
{
  m_enabled = true;
  return ;
}

void	PouetEffect::disable( void )
{
  m_enabled = false;
  return ;
}
