#include "EffectsEngine.h"
#include "GenericEffect.h"
#include "PouetEffect.h"

int main(void)
{
  quint32	i;
  LightVideoFrame	f;
  QHash<int, OutSlot<LightVideoFrame> >	qh;
  InSlot<LightVideoFrame>		in;

  for (i = 0; i < 30; ++i)
    {
      qh[i];
    }
  qh[0].connect(in);
//   qh[0] << f;
//   qh[1] = f;
  return (0);
}
