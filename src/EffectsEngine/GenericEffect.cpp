#include "GenericEffect.h"

GenericEffect::GenericEffect( char const * videoinputs[], quint32 const nbvideoinputs,
			      char const * videooutputs[], quint32 const nbvideooutputs)
{
  m_videoInputs = new InSlot<LightVideoFrame>[nbvideoinputs];
  m_videoOutputs = new OutSlot<LightVideoFrame>[nbvideooutputs];
}

GenericEffect::~GenericEffect()
{
  delete m_videoInputs;
  delete m_videoOutputs;
}


// CONNECTIONS BETWEEN GENERICEFFECTS

void				GenericEffect::connectOutput( quint32 outIndex, GenericEffect* destEffect, quint32 inIndex)
{
  // THINK TO CHECK IF THE SLOTS EXISTS BY CALLING THE PRIVATES METHODS!!!
  ( m_videoOutputs[outIndex] ).connect( destEffect->m_videoInputs[inIndex] );
  return ;
}

// CONNECTIONS DETWEEN GENERICEFFECT & OUTSLOT/INSLOT

void				GenericEffect::connect( OutSlot<LightVideoFrame> & out, quint32 inIndex )
{
  out.connect( m_videoInputs[inIndex] );
  return ;
}

void				GenericEffect::connect( quint32 outIndex, InSlot<LightVideoFrame> & in )
{
  ( m_videoOutputs[outIndex] ).connect( in );
  return ;
}

//
// PRIVATES METHODS
//

// InSlot<LightVideoFrame>&	GenericEffect::getStaticVideoInput( QString const & name )
// {
//   return ( m_videoInputs[name] );
// }

// OutSlot<LightVideoFrame>&	GenericEffect::getStaticVideoOutput( QString const & name )
// {
//   return ( m_videoOutputs[name] );
// }
