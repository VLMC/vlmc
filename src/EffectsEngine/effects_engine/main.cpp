#include "EffectsEngine.h"
#include "GenericEffect.h"
#include "PouetEffect.h"

int main(void)
{
//   quint32	i;
  EffectsEngine		ee;
  VideoFrame		vf1((quint8*)"123456789", 9);
  VideoFrame		vf2((quint8*)"ABCDEFGHI", 9);

  ee.setInputFrame(vf1, 0);
  ee.setInputFrame(vf2, 1);
  ee.render();

  std::cout << "result : " << ee.getOutputFrame(0).rvf.raw << std::endl;

//   VideoFrame		nvf((quint8*)"1234abcdABCD", 12);

//   for ( i = 0; i < nvf.nbpixels; ++i )
//     {
//       std::cout << "Pixel[" << i << "].Red = " << nvf.rvf.pixel[i].Red << std::endl;
//       std::cout << "Pixel[" << i << "].Green = " << nvf.rvf.pixel[i].Green << std::endl;
//       std::cout << "Pixel[" << i << "].Blue = " << nvf.rvf.pixel[i].Blue << std::endl;
//     }

  return (0);
}
