#ifndef EFFECTSENGINE_H_
#define EFFECTSENGINE_H_

#include <QtGlobal>
#include <QHash>
#include <iostream>
#include <QReadWriteLock>

#include "LightVideoFrame.h"
#include "LightParameter.h"
#include "InSlot.hpp"
#include "OutSlot.hpp"
#include "GenericEffect.h"
#include "PouetEffect.h"
#include "MixerEffect.h"

class	EffectsEngine
{

 public:

  // CTOR & DTOR

  EffectsEngine( void 
		/* quint32 nbinputs, quint32 nboutputs  */);
  ~EffectsEngine();

  // MAIN METHOD

  void		render( void );

  // BYPASSING

  void		enable( void );
  void		disable( void );

  // INPUTS & OUTPUTS METHODS

/*   void			setClock( Parameter currentframenumber ); */
  void				setInputFrame( LightVideoFrame& frame, quint32 tracknumber );
  LightVideoFrame const &       getOutputFrame( quint32 tracknumber ) const;

 private:
  
  // START & STOP

  void		start( void );
  void		stop( void );

  // EFFECTS LOADING & UNLOADING


  void		loadEffects( void );
  void		unloadEffects( void );

  // EFFECTS PATCHING

  void		patchEffects( void );


 private:

  QHash< quint32, GenericEffect* >              m_effects;
  QHash< quint32, OutSlot<LightVideoFrame> >	m_videoInputs; // It's OutSlots because, it's the Outputs of the workflow, that should be connected to InSlots of effects
  QHash< quint32, InSlot<LightVideoFrame> >	m_videoOutputs; // It's InSlots because, it's the Inputs of the effect engine, that should be connected to OutSlots of the renderer
/*   OutSlot<LightParameter>			m_clockInput;	 // It's OutSlots because, it's the Outputs of the clock of the workflow, that should be connected to OutSlots */

  QReadWriteLock*                               m_inputLock;

};

#endif // EFFECTSENGINE_H_
