#ifndef EFFECTSENGINE_H_
#define EFFECTSENGINE_H_

#include <QtGlobal>
#include <iostream>
#include "LightVideoFrame.h"
#include "LightParameter.h"
#include "InSlot.hpp"
#include "OutSlot.hpp"


class	EffectsEngine
{

 public:

  // CTOR & DTOR

  EffectsEngine();
  ~EffectsEngine();

  // INITIALIZATION

  void		init(unsigned int nbinputs, unsigned int nboutputs);

  // MAIN METHOD

  void		doTheMagic(void);

  // INPUTS & OUTPUTS METHODS

  void			setClock(LightParameter currentframenumber);
  void			setInputFrame(LightVideoFrame frame, unsigned int tracknumber);
  LightVideoFrame&	getOutputFrame(unsigned int tracknumber) const;

 private:

  void			initPatch(void);

 private:

  unsigned int		m_nbInputs;
  unsigned int		m_nbOutputs;
  OutSlot<LightVideoFrame>*	m_inputsVideoFrames; // It's OutSlots because, it's the Outputs of the workflow, that should be connected to InSlots of effects
  InSlot<LightVideoFrame>*	m_outputsVideoFrames; // It's InSlots because, it's the Inputs of the effect engine, that should be connected to OutSlots of the renderer
  OutSlot<LightParameter>	m_inputClock;	 // It's OutSlots because, it's the Outputs of the clock of the workflow, that should be connected to OutSlots

};

#endif // EFFECTSENGINE_H_
