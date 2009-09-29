#include "MixerEffect.h"
#include <QtDebug>

char const *	MixerEffect::m_videoInputsNames[] =
  {
    "track0",
    "track1",
    "track2",
    "track3",
    "track4",
    "track5",
    "track6",
    "track7",
    "track8",
    "track9",
    "track10",
    "track11",
    "track12",
    "track13",
    "track14",
    "track15",
    "track16",
    "track17",
    "track18",
    "track19",
    "track20",
    "track21",
    "track22",
    "track23",
    "track24",
    "track25",
    "track26",
    "track27",
    "track28",
    "track29",
    "track30",
    "track31",
    "track32",
    "track33",
    "track34",
    "track35",
    "track36",
    "track37",
    "track38",
    "track39",
    "track40",
    "track41",
    "track42",
    "track43",
    "track44",
    "track45",
    "track46",
    "track47",
    "track48",
    "track49",
    "track50",
    "track51",
    "track52",
    "track53",
    "track54",
    "track55",
    "track56",
    "track57",
    "track58",
    "track59",
    "track60",
    "track61",
    "track62",
    "track63"
  };
char const *	MixerEffect::m_videoOutputsNames[] = { "out" };

MixerEffect::MixerEffect() : GenericEffect(
					   MixerEffect::m_videoInputsNames, MixerEffect::m_nbVideoInputs,
					   MixerEffect::m_videoOutputsNames, MixerEffect::m_nbVideoOutputs
					   )
{
};

MixerEffect::~MixerEffect()
{
}

void	MixerEffect::render( void )
{
  quint32	i;
  QHash< QString, InSlot<LightVideoFrame> >::iterator   it = m_videoInputs.begin();
  QHash< QString, InSlot<LightVideoFrame> >::iterator   end = m_videoInputs.end();


  while ( it != end )
  {
      const VideoFrame&   lvf = static_cast<VideoFrame>( static_cast<LightVideoFrame>( ( it.value() ) ) );
      if ( lvf.rvf.raw != NULL )
      {
          m_videoOutputs["out"] << lvf;
	  return ;
      }
      ++it;
  }
  return ;
}
