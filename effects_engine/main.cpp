#include "EffectsEngine.h"
#include "GenericEffect.h"
#include "PouetEffect.h"

int main(void)
{
  GenericEffect*	effect = new PouetEffect();
  OutSlot<LightVideoFrame>		in;
  InSlot<LightVideoFrame>		out;
  VideoFrame				poney((char const *)("PONEY"));
  LightVideoFrame			truc(poney);
  LightVideoFrame			truc2;
  
  in.connect(effect->getVideoInput(QString("in")));
  (effect->getVideoOutput(QString("out"))).connect(out);

  in = truc;
  effect->render();
  truc2 = out;
  poney = truc2;
  char const * result = poney;
  std::cout << "result : " << result << std::endl;
  return (0);
}
