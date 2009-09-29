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
//  for ( i = 0; i < tmp.nboctets; ++i )
//    {
//      qDebug() << "DUMP"
// 	      << "tmp.rvf.raw[" << i << "] = "
// 	      << tmp.rvf.raw[i];
//    }

 for ( i = 0; i < tmp.nbpixels; ++i )
   {
//      qDebug() << "BEFORE MODIFIYING COLORS"
// 	      << "tmp.rvf.pixel[i].Red = "
// 	      <<  tmp.rvf.pixel[i].Red
// 	      << '\n'
// 	      << "tmp.rvf.pixel[i].Blue = "
// 	      <<  tmp.rvf.pixel[i].Blue
// 	      << "\n-----------------------";

     tmp.rvf.pixel[i].Red = 0;
     tmp.rvf.pixel[i].Blue = 0;
//      qDebug() << "AFTER MODIFIYING COLORS"
// 	      << "tmp.rvf.pixel[i].Red = "
// 	      <<  tmp.rvf.pixel[i].Red
// 	      << '\n'
// 	      << "tmp.rvf.pixel[i].Blue = "
// 	      <<  tmp.rvf.pixel[i].Blue
// 	      << "\n-----------------------";
   }
 (m_videoOutputs["out"]) << tmp;
  qDebug() << "End of PouetEffect";
  return ;
}
