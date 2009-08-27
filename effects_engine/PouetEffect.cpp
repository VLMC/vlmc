#include "PouetEffect.h"

char const * 	PouetEffect::m_videoInputsNames[] = {"vin1", "vin2", "vin3"};
char const *	PouetEffect::m_videoOutputsNames[] = {"vout1", "vout2", "vout3"};
 
PouetEffect::PouetEffect() : GenericEffect(
					   PouetEffect::m_videoInputsNames, PouetEffect::m_nbVideoInputs,
					   PouetEffect::m_videoOutputsNames, PouetEffect::m_nbVideoOutputs
					   )
{
};

PouetEffect::~PouetEffect()
{
}

void	PouetEffect::render(void)
{
  return ;
}
