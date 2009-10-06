#include "LightVideoFrame.h"

VideoFrame::~VideoFrame()
{
  if ( frame.octets != NULL )
    delete [] frame.octets;
};

VideoFrame::VideoFrame()
{
  frame.octets = NULL;
  nboctets = 0;
  nbpixels = 0;
}

VideoFrame::VideoFrame(VideoFrame const & tocopy) : QSharedData( tocopy )
{
  if ( tocopy.frame.octets != NULL )
    {
      quint32	i;
	
      nboctets = tocopy.nboctets;
      nbpixels = tocopy.nboctets / Pixel::NbComposantes;
      frame.octets = new quint8[tocopy.nboctets];
	
      for ( i = 0; i < nboctets; ++i )
	frame.octets[i] = tocopy.frame.octets[i];
    }
  else
    {
      nboctets = 0;
      nbpixels = 0;
      frame.octets = NULL;
    }
}

//
//
//
//
//

LightVideoFrame::LightVideoFrame()
{
  m_videoFrame = new VideoFrame;
};

LightVideoFrame::LightVideoFrame(LightVideoFrame const & tocopy) : m_videoFrame(tocopy.m_videoFrame)
{
};

LightVideoFrame&	LightVideoFrame::operator=(LightVideoFrame const & tocopy)
{
//  if ( m_videoFrame->frame.octets == NULL )
//    {
//      if ( tocopy.m_videoFrame->frame.octets != NULL )
//        {
// 	 qDebug() << "frame NULL & frame of tocopy not NULL" << "\n"
// 		  << "this pixels = " << m_videoFrame->nbpixels << "\n"
// 		  << "tocopy.m_videoFrame pixels = " << tocopy.m_videoFrame->nbpixels;
// 	 m_videoFrame->frame.octets = new quint8[tocopy.m_videoFrame->nboctets];
// 	 m_videoFrame->nboctets = tocopy.m_videoFrame->nboctets;
// 	 m_videoFrame->nbpixels = tocopy.m_videoFrame->nboctets / Pixel::NbComposantes;
// 	 quint32	i;
	 
// 	 for ( i = 0; i < m_videoFrame->nboctets; ++i )
// 	   m_videoFrame->frame.octets[i] = tocopy.m_videoFrame->frame.octets[i];
//        }
//    }
//  else
//    {
//      if ( tocopy.m_videoFrame->frame.octets != NULL )
//        {
// 	 qDebug() << "frame not NULL & frame of tocopy not NULL" << "\n"
// 		  << "this pixels = " << m_videoFrame->nbpixels << "\n"
// 		  << "tocopy.m_videoFrame pixels = " << tocopy.m_videoFrame->nbpixels;
// 	 if ( m_videoFrame->nboctets != tocopy.m_videoFrame->nboctets )
// 	   {
// 	     delete [] m_videoFrame->frame.octets;
// 	     m_videoFrame->frame.octets = new quint8[tocopy.m_videoFrame->nboctets];
// 	     m_videoFrame->nboctets = tocopy.m_videoFrame->nboctets;
// 	     m_videoFrame->nbpixels = tocopy.m_videoFrame->nboctets / Pixel::NbComposantes;
// 	   }
// 	 quint32	i;
	 
// 	 for ( i = 0; i < m_videoFrame->nboctets; ++i )
// 	   m_videoFrame->frame.octets[i] = tocopy.m_videoFrame->frame.octets[i];
//        }
//      else
//        {
// 	 qDebug() << "frame not NULL & frame of tocopy NULL" << "\n"
// 		  << "this pixels = " << m_videoFrame->nbpixels << "\n"
// 		  << "tocopy.m_videoFrame pixels = " << tocopy.m_videoFrame->nbpixels;
// 	 delete [] m_videoFrame->frame.octets;
// 	 m_videoFrame->frame.octets = NULL;
// 	 m_videoFrame->nboctets = 0;
// 	 m_videoFrame->nbpixels = 0;
//        }
//    }

  //
  //
  // #FIXME : WTF!? Why does this holly crap work fine?
  //
  //

  m_videoFrame.detach();
  m_videoFrame = tocopy.m_videoFrame;
  return ( *this );
};

LightVideoFrame::LightVideoFrame(quint32 nboctets)
{
  m_videoFrame = new VideoFrame;
  m_videoFrame->nboctets = nboctets;
  m_videoFrame->nbpixels = nboctets / Pixel::NbComposantes;
  m_videoFrame->frame.octets = new quint8[nboctets];
};

LightVideoFrame::LightVideoFrame(quint8 const * tocopy, quint32 nboctets)
{
  quint32	i;

  m_videoFrame = new VideoFrame;
  m_videoFrame->nboctets = nboctets;
  m_videoFrame->nbpixels = nboctets / Pixel::NbComposantes;
  m_videoFrame->frame.octets = new quint8[nboctets];

  for ( i = 0; i < m_videoFrame->nboctets; ++i )
    m_videoFrame->frame.octets[i] = tocopy[i];
};

LightVideoFrame::~LightVideoFrame()
{
};
  
VideoFrame const * LightVideoFrame::operator->(void) const
{
  return ( m_videoFrame.data() );
};

VideoFrame const & LightVideoFrame::operator*(void) const
{
  return ( *m_videoFrame );
};

VideoFrame* LightVideoFrame::operator->(void)
{
  return ( m_videoFrame.data() );
};

VideoFrame& LightVideoFrame::operator*(void)
{
  return ( *m_videoFrame );
};
