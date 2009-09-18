#ifndef VIDEOFRAME_H_
#define VIDEOFRAME_H_

#include	<QtGlobal>
// #include <QByteArray>

// typedef	QByteArray	VideoFrame;

struct	Pixel
{
  enum
    {
      NbComposantes = 3
    };
  quint8	Red;
  quint8	Green;
  quint8	Blue;
};

union	RawVideoFrame
{
  Pixel*	pixel;
  quint8*	raw;
};

struct	VideoFrame
{
  ~VideoFrame();
  VideoFrame();
  VideoFrame(quint8* tocopy, quint32 size);
  VideoFrame(VideoFrame const &);
  VideoFrame & operator=(VideoFrame const & tocopy);
  
  RawVideoFrame	rvf;
  quint32	nbpixels;
  quint32	nboctets;
};

#endif // VIDEOFRAME_H_
