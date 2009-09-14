#include "PouetEffect.h"
#include <QtDebug>

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
  qDebug() << "---------->PouetEffect";
  quint32		i;
  LightVideoFrame	tmp;
  quint32		size;

//   m_videoOutputs["out"] << m_videoInputs["in"];

 ( m_videoInputs["in"] ) >> tmp;
 for ( i = 0; i < tmp.nbpixels; ++i )
   {
     tmp.rvf.pixel[i].Red = 0;
     tmp.rvf.pixel[i].Blue = 0;
   }
 (m_videoOutputs["out"]) << tmp;
  qDebug() << "End of PouetEffect";
  return ;
}
