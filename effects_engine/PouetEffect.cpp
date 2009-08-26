#include "PouetEffect.h"

PouetEffect::PouetEffect() : GenericEffect(
					   PouetEffect::m_videoInputs, PouetEffect::m_nbVideoInputs,
					   PouetEffect::m_videoOutputs, PouetEffect::m_nbVideoOutputs
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
