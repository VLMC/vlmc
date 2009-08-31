#include "EffectsEngine.h"
#include "GenericEffect.h"
#include "PouetEffect.h"

int main(void)
{
//   GenericEffect*	effect1 = new PouetEffect();
//   GenericEffect*	effect2 = new PouetEffect();
//   OutSlot<LightVideoFrame>		in;
//   InSlot<LightVideoFrame>		out;
//   VideoFrame				poney( (char const *)( "1234567890" ) );
//   LightVideoFrame			truc( poney );
//   LightVideoFrame			truc2;
  

//   // CONNECTIONS

//   effect1->connect( in, QString( "in" ) );

//   effect1->connectOutput( QString( "out" ), effect2, QString( "in" ) );

//   effect2->connect( QString( "out" ), out );

//   // SEND INPUT

//   in << truc;

//   // RENDER

//   effect1->render();
//   effect2->render();

//   // RECEIVE OUTPUT

//   out >> truc2;

//   // CONVERSION AND PRINTING TO STDOUT

//   poney = truc2;
//   char const * result = poney;
//   std::cout << "result : " << result << std::endl;
  EffectsEngine		ee;

  ee.setInputFrame("1234567890", 0);
  ee.setInputFrame("ABCDEFGHIJKL", 1);
  ee.render();
  std::cout << "result : " << static_cast<char const *>(ee.getOutputFrame(0)) << std::endl;
  return (0);
}
