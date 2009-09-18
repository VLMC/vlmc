#include "EffectsEngine.h"

// CTOR & DTOR

EffectsEngine::EffectsEngine( void//  quint32 nbinputs, quint32 nboutputs 
			      )
{
   quint32	i;

  for (i = 0; i < 64; ++i)
    m_videoInputs[i];
  for (i = 0; i < 1; ++i)
    m_videoOutputs[i];       
  start();
//    quint32	i;

//   for (i = 0; i < nbinputs; ++i)
//     m_videoInputs[i];
//   for (i = 0; i < nboutputs; ++i)
//     m_videoOutputs[i];       
}

EffectsEngine::~EffectsEngine()
{
  stop();
}

// MAIN METHOD

void	EffectsEngine::render( void )
{
  ( m_effects[0] )->render();
  ( m_effects[1] )->render();
  return ;
}


// INPUTS & OUTPUTS METHODS

// void	EffectsEngine::setClock( Parameter currentframenumber )
// { 
//  std::cout << "setClock" << std::endl;
//   return ;
// }

void	EffectsEngine::setInputFrame( VideoFrame& frame, quint32 tracknumber )
{
  m_videoInputs[tracknumber] = frame;
  return ;
}

// TO REPLACE BY A REF

VideoFrame const &	EffectsEngine::getOutputFrame( quint32 tracknumber ) const
{
  return ( ( VideoFrame const & )( ( LightVideoFrame const & ) (m_videoOutputs[tracknumber] ) ) );
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
  unloadEffects();
  return ;
}

// EFFECTS LOADING & UNLOADING

void	EffectsEngine::loadEffects( void )
{
  m_effects[0] = new MixerEffect();
  m_effects[1] = new PouetEffect();
  return ;
}

void	EffectsEngine::unloadEffects( void )
{
  delete m_effects[0];
  delete m_effects[1];
  return ;
}

// EFFECTS PATCHING

void	EffectsEngine::patchEffects( void )
{
  quint32	i;
  QString	tmp;

  for ( i = 0; i < 64; ++i )
    {
      tmp = "track" + QString::number(i);
      ( m_effects[0] )->connect( m_videoInputs[i], tmp );
    }
  ( m_effects[0] )->connectOutput( QString( "out" ) , m_effects[1], QString( "in" ) );
  ( m_effects[1] )->connect( QString( "out" ), m_videoOutputs[0] );
  return ;
}

