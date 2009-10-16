#include "GreenFilterEffect.h"
#include <QtDebug>

char const * 	GreenFilterEffect::m_videoInputsNames[] = {"in"};
char const *	GreenFilterEffect::m_videoOutputsNames[] = {"out"};
 
GreenFilterEffect::GreenFilterEffect() : GenericEffect(
					   GreenFilterEffect::m_videoInputsNames, GreenFilterEffect::m_nbVideoInputs,
					   GreenFilterEffect::m_videoOutputsNames, GreenFilterEffect::m_nbVideoOutputs
					   ),
			     m_enabled( true )
{
};

GreenFilterEffect::~GreenFilterEffect()
{
}

void    GreenFilterEffect::render( void )
{
  quint32		i;
  LightVideoFrame	tmp;
  
  ( m_videoInputs[0] ) >> tmp;
  if (tmp->frame.octets != NULL)
  {
    if (m_enabled)
      for ( i = 0; i < tmp->nbpixels; ++i )
	{
	  tmp->frame.pixels[i].Red = 0;
	  tmp->frame.pixels[i].Blue = 0;
	}
    (m_videoOutputs[0]) << tmp;
  }
  return ;
}

void	GreenFilterEffect::enable( void )
{
  m_enabled = true;
  return ;
}

void	GreenFilterEffect::disable( void )
{
  m_enabled = false;
  return ;
}
