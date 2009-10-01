#include "VideoFrame.h"
#include <QtDebug>

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

VideoFrame::VideoFrame( quint32 nboctets )
{
  this->nboctets = nboctets;
  this->nbpixels = nboctets / Pixel::NbComposantes;
  this->rvf.raw = new quint8[nboctets];
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
      qDebug() << __LINE__;
      if ( tocopy.rvf.raw != NULL )
	{
	  qDebug() << __LINE__;
	  if ( this->nboctets != tocopy.nboctets )
	    {
	      qDebug() << __LINE__ << "\n"
		       << "this pixels = " << this->nbpixels
		       << "tocopy pixels = " << tocopy.nbpixels;
	      delete [] this->rvf.raw;
	      this->rvf.raw = new quint8[tocopy.nboctets];
	      this->nboctets = tocopy.nboctets;
	      this->nbpixels = tocopy.nboctets / Pixel::NbComposantes;
	      qDebug() << __LINE__;
	    }
	  quint32	i;
	  
	  qDebug() << __LINE__;
	  for ( i = 0; i < this->nboctets; ++i )
	    this->rvf.raw[i] = tocopy.rvf.raw[i];
	  qDebug() << __LINE__;
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
  if (tocopy.rvf.raw != NULL)
    {
      quint32	i;

      this->nboctets = tocopy.nboctets;
      this->nbpixels = tocopy.nboctets / Pixel::NbComposantes;
      this->rvf.raw = new quint8[tocopy.nboctets];

      for ( i = 0; i < this->nboctets; ++i )
	this->rvf.raw[i] = tocopy.rvf.raw[i];
    }
  else
    {
      this->nboctets = 0;
      this->nbpixels = 0;
      this->rvf.raw = NULL;
    }
}
