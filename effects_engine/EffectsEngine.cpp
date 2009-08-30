#include "EffectsEngine.h"

// CTOR & DTOR

EffectsEngine::EffectsEngine( quint32 nbinputs, quint32 nboutputs )
{
   quint32	i;

  for (i = 0; i < nbinputs; ++i)
    m_videoInputs[i];
  for (i = 0; i < nboutputs; ++i)
    m_videoOutputs[i];       
}

EffectsEngine::~EffectsEngine()
{
}

// MAIN METHOD

void	EffectsEngine::doTheMagic( void )
{
  std::cout << "doTheMagic" << std::endl;
  return ;
}


// INPUTS & OUTPUTS METHODS

void	EffectsEngine::setClock( Parameter currentframenumber )
{ 
 std::cout << "setClock" << std::endl;
  return ;
}

void	EffectsEngine::setInputFrame( VideoFrame frame, quint32 tracknumber )
{
  std::cout << "setInputFrame" << std::endl;
  return ;
}


// TO REPLACE BY A REF

LightVideoFrame	EffectsEngine::getOutputFrame( quint32 tracknumber ) const
{
  return ( m_videoOutputs[tracknumber] );
}


//
// PRIVATES METHODS
//


// START & STOP

void	EffectsEngine::start( void )
{
  loadEffects();
  patchEffects();
  return ;
}

void	EffectsEngine::stop( void )
{
  return ;
}

// EFFECTS LOADING & UNLOADING

void	EffectsEngine::loadEffects( void )
{
  return ;
}

void	EffectsEngine::unloadEffects( void )
{
  return ;
}

// EFFECTS PATCHING

void	EffectsEngine::patchEffects( void )
{
  return ;
}

