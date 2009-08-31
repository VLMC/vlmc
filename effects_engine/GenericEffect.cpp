#include "GenericEffect.h"

GenericEffect::GenericEffect( char const * videoinputs[], quint32 const nbvideoinputs,
			      char const * videooutputs[], quint32 const nbvideooutputs)
{
  quint32	i;

  for (i = 0; i < nbvideoinputs; ++i)
    m_videoInputs[videoinputs[i]];
  for (i = 0; i < nbvideooutputs; ++i)
    m_videoOutputs[videooutputs[i]];      
}

GenericEffect::~GenericEffect()
{
}


// CONNECTIONS BETWEEN GENERICEFFECTS

void				GenericEffect::connectOutput( QString const & outName, GenericEffect* destEffect, QString const & inName)
{
  // THINK TO CHECK IF THE SLOTS EXISTS BY CALLING THE PRIVATES METHODS!!!
  ( m_videoOutputs[outName] ).connect( destEffect->m_videoInputs[inName] );
  return ;
}

// CONNECTIONS DETWEEN GENERICEFFECT & OUTSLOT/INSLOT

void				GenericEffect::connect( OutSlot<LightVideoFrame> & out, QString const & inName )
{
  out.connect( m_videoInputs[inName] );
  return ;
}

void				GenericEffect::connect( QString const & outName, InSlot<LightVideoFrame> & in )
{
  ( m_videoOutputs[outName] ).connect( in );
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
