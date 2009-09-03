#include "VideoFrame.h"

VideoFrame::~VideoFrame()
{
  if ( this->rvf.raw != NULL )
    delete [] this->rvf.raw;
}

VideoFrame::VideoFrame()
{
  this->rvf.raw = NULL;
  this->nboctets = 0;
  this->nbpixels = 0;
}

VideoFrame::VideoFrame( quint8* tocopy, quint32 nboctets )
{
  quint32	i;

  this->nboctets = nboctets;
  this->nbpixels = nboctets / Pixel::NbComposantes;
  this->rvf.raw = new quint8[nboctets];

  for ( i = 0; i < this->nboctets; ++i )
    this->rvf.raw[i] = tocopy[i];
}

VideoFrame & VideoFrame::operator=( VideoFrame const & tocopy )
{
  if ( this->rvf.raw == NULL )
    {
      if ( tocopy.rvf.raw != NULL )
	{
	  this->rvf.raw = new quint8[tocopy.nboctets];
	  this->nboctets = tocopy.nboctets;
	  this->nbpixels = tocopy.nboctets / Pixel::NbComposantes;
	  quint32	i;
	  
	  for ( i = 0; i < this->nboctets; ++i )
	    this->rvf.raw[i] = tocopy.rvf.raw[i];
	}
    }
  else
    {
      if ( tocopy.rvf.raw != NULL )
	{
	  if ( this->nboctets != tocopy.nboctets )
	    {
	      delete [] this->rvf.raw;
	      this->rvf.raw = new quint8[tocopy.nboctets];
	      this->nboctets = tocopy.nboctets;
	      this->nbpixels = tocopy.nboctets / Pixel::NbComposantes;
	    }
	  quint32	i;
	  
	  for ( i = 0; i < this->nboctets; ++i )
	    this->rvf.raw[i] = tocopy.rvf.raw[i];
	}
      else
	{
	  delete [] this->rvf.raw;
	  this->rvf.raw = NULL;
	  this->nboctets = 0;
	  this->nbpixels = 0;
	}
    }
 
 return ( *this );
}

VideoFrame::VideoFrame( VideoFrame const & tocopy )
{
  quint32	i;

  this->nboctets = tocopy.nboctets;
  this->nbpixels = tocopy.nboctets / Pixel::NbComposantes;
  this->rvf.raw = new quint8[tocopy.nboctets];

  for ( i = 0; i < this->nboctets; ++i )
    this->rvf.raw[i] = tocopy.rvf.raw[i];
}
