#ifndef GENERICEFFECT_H_
#define GENERICEFFECT_H_

#include <QtGlobal>
#include <QString>
#include <QHash>

#include "InSlot.hpp"
#include "OutSlot.hpp"
#include "LightVideoFrame.h"

class	GenericEffect
{
 public:
 
 // CTOR & DTOR

  virtual ~GenericEffect();
  GenericEffect( char const * videoinputs[], quint32 const nbvideoinputs,
		 char const * videooutputs[], quint32 const nbvideooutputs);

  // RENDER METHOD

  virtual void  render(void) = 0;

 private:

  QHash< QString, InSlot<LightVideoFrame> >	m_videoInputs;
  QHash< QString, OutSlot<LightVideoFrame> >	m_videoOutputs;
/*   QHash< QString, InSlot<SoundSampleFrame> >	m_soundInputs; */
/*   QHash< QString, OutSlot<SoundSampleFrame> >m_soundOutputs; */
/*   QHash< QString, InSlot<LightVideoFrame> >	m_controlInputs; */
/*   QHash< QString, OutSlot<LightVideoFrame> >	m_controlOutputs; */
  
};

#endif // GENERICEFFECT_H_
