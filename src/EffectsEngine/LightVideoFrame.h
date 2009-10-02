#ifndef LIGHTVIDEOFRAME_H_
#define LIGHTVIDEOFRAME_H_

#include <QSharedDataPointer>
#include <QSharedData>
#include <QDebug>

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
  Pixel*	pixels;
  quint8*	octets;
};

struct	VideoFrame : public QSharedData
{
  ~VideoFrame();
  VideoFrame();
  VideoFrame(VideoFrame const & tocopy);

  RawVideoFrame	frame;
  quint32	nbpixels;
  quint32	nboctets;
};

class	LightVideoFrame
{
public:

  LightVideoFrame();
  LightVideoFrame(LightVideoFrame const & tocopy);
  LightVideoFrame(quint32 nboctets);
  LightVideoFrame(quint8 const * tocopy, quint32 nboctets);
  ~LightVideoFrame();

  LightVideoFrame&	operator=(LightVideoFrame const & tocopy);
  VideoFrame const * operator->(void) const;
  VideoFrame const & operator*(void) const;
  VideoFrame* operator->(void);
  VideoFrame& operator*(void);
  
private:

  QSharedDataPointer<VideoFrame>	m_videoFrame;

};

#endif // VIDEOFRAME_H_
