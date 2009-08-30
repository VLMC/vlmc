#include "EffectsEngine.h"
#include "GenericEffect.h"
#include "PouetEffect.h"

int main(void)
{
  GenericEffect*	effect1 = new PouetEffect();
  GenericEffect*	effect2 = new PouetEffect();
  OutSlot<LightVideoFrame>		in;
  InSlot<LightVideoFrame>		out;
  VideoFrame				poney( (char const *)( "1234567890" ) );
  LightVideoFrame			truc( poney );
  LightVideoFrame			truc2;
  

  // CONNECTIONS

  in.connect( effect1->getVideoInput( QString( "in" ) ) );
  ( effect1->getVideoOutput( QString( "out" ) ) ).connect( effect2->getVideoInput( QString( "in" ) ) );
  ( effect2->getVideoOutput( QString( "out" ) ) ).connect( out );

  // SEND INPUT

  in << truc;

  // RENDER

  effect1->render();
  effect2->render();

  // RECEIVE OUTPUT

  out >> truc2;

  // CONVERSION AND PRINTING TO STDOUT

  poney = truc2;
  char const * result = poney;
  std::cout << "result : " << result << std::endl;
  return (0);
}
