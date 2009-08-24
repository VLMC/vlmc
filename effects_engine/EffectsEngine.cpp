#include "EffectsEngine.h"

// CTOR & DTOR

EffectsEngine::EffectsEngine()
{
}

EffectsEngine::~EffectsEngine()
{
}

// INITIALIZATION

void	EffectsEngine::init(unsigned int nbinputs, unsigned int nboutputs)
{
  std::cout << "init" << std::endl;
  m_inputsVideoFrames = new OutSlot<LightVideoFrame>[nbinputs];
  m_outputsVideoFrames = new InSlot<LightVideoFrame>[nboutputs];
  return ;
}
// MAIN METHOD

void	EffectsEngine::doTheMagic(void)
{
  std::cout << "doTheMagic" << std::endl;
  return ;
}


// INPUTS & OUTPUTS METHODS

void	EffectsEngine::setClock(LightParameter currentframenumber)
{ 
 std::cout << "setClock" << std::endl;
  return ;
}

void	EffectsEngine::setInputFrame(LightVideoFrame frame, unsigned int tracknumber)
{
  std::cout << "setInputFrame" << std::endl;
  return ;
}

LightVideoFrame&	EffectsEngine::getOutputFrame(unsigned int tracknumber) const
{
  return ((LightVideoFrame&)(m_outputsVideoFrames[tracknumber]));
}

//
// PRIVATE METHODS
//

void		EffectsEngine::initPatch(void)
{
  return ;
}
