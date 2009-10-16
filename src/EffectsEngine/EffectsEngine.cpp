#include <QtDebug>
#include "EffectsEngine.h"

// CTOR & DTOR

EffectsEngine::EffectsEngine( void )
{
   quint32	i;

   m_inputLock = new QReadWriteLock;
   m_videoInputs = new OutSlot<LightVideoFrame>[64];
   m_videoOutputs = new InSlot<LightVideoFrame>[64];       
   start();
}

EffectsEngine::~EffectsEngine()
{
    stop();
    delete [] m_videoInputs;
    delete [] m_videoOutputs;
    delete m_inputLock;
}

// MAIN METHOD

void	EffectsEngine::render( void )
{
  QWriteLocker    lock( m_inputLock );
  ( m_effects[0] )->render();
  ( m_effects[1] )->render();
  return ;
}

// BYPASSING

void		EffectsEngine::enable( void )
{
  QWriteLocker    lock( m_inputLock );
  reinterpret_cast<GreenFilterEffect*>(m_effects[1])->enable(); // YES, I KNOW, IT'S HUGLY, BUT IT'S TEMPORARY
}

void		EffectsEngine::disable( void )
{
  QWriteLocker    lock( m_inputLock );
  reinterpret_cast<GreenFilterEffect*>(m_effects[1])->disable(); // YES, I KNOW, IT'S HUGLY, BUT IT'S TEMPORARY (second time)
}

// INPUTS & OUTPUTS METHODS

// void	EffectsEngine::setClock( Parameter currentframenumber )
// { 
//  std::cout << "setClock" << std::endl;
//   return ;
// }

void	EffectsEngine::setInputFrame( LightVideoFrame& frame, quint32 tracknumber )
{
    QWriteLocker    lock( m_inputLock );

    m_videoInputs[tracknumber] = frame;
    return ;
}

// TO REPLACE BY A REF

LightVideoFrame const &	EffectsEngine::getOutputFrame( quint32 tracknumber ) const
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
  unloadEffects();
  return ;
}

// EFFECTS LOADING & UNLOADING

void	EffectsEngine::loadEffects( void )
{
  m_effects[0] = new MixerEffect();
  m_effects[1] = new GreenFilterEffect();
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

    QReadLocker lock( m_inputLock );
    for ( i = 0; i < 64; ++i )
    {   
        m_effects[0]->connect( m_videoInputs[i], i );
    }
    m_effects[0]->connectOutput( 0 , m_effects[1], 0 );
    m_effects[1]->connect( 0, m_videoOutputs[0] );
    return ;
}
