// #include "EffectsEngine.h"
// #include "GenericEffect.h"
// #include "PouetEffect.h"

#include <QSharedDataPointer>
#include <QSharedData>
#include <QDebug>

// class EmployeeData : public QSharedData
// {
// public:
//   EmployeeData() : id(-1) { name.clear(); }
//   EmployeeData(const EmployeeData &other)
//     : QSharedData(other), id(other.id), name(other.name) { }
//   ~EmployeeData() { }

//   int id;
//   QString name;
// };

// class Employee
// {
// public:
//   Employee() { d = new EmployeeData; }
//   Employee(int id, QString name) {
//     d = new EmployeeData;
//     setId(id);
//     setName(name);
//   }
//    Employee(const Employee &other)
//      : d (other.d)
//   {
//   }
  
  
//   void setId(int id) { d->id = id; }
//   void setName(QString name) { d->name = name; }
  
//   int id() const { return d->id; }
//   QString name() const { return d->name; }
  
// private:
//   QSharedDataPointer<EmployeeData> d;
// };

//
//
//
//
//

// union	PPixel
// {
//   enum
//     {
//       NbComposantes = 3
//     };
//   struct
//   {
//     quint8	Red;
//     quint8	Green;
//     quint8	Blue;
//   }		composantes;
//   quint32	raw : NbComposantes * 8;
// };

// struct	PVideoFrame
// {
// //   ~VideoFrame();
// //   VideoFrame();

//   //  VideoFrame(quint32 nboctets);
//   //  VideoFrame(quint8* tocopy, quint32 nboctets);

// //   VideoFrame(VideoFrame const &);
// //   VideoFrame & operator=(VideoFrame const & tocopy);
  
//   union
//   {
//     PPixel*	pixels;
//     quint8*	octets;
//   }		frame;
//   quint32	nbpixels;
//   quint32	nboctets;
// };


//
//
//
//
//

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
  ~VideoFrame()
  {
    if ( this->frame.octets != NULL )
      delete [] frame.octets;
  };

  VideoFrame()
  {
    this->frame.octets = NULL;
    this->nboctets = 0;
    this->nbpixels = 0;
  }

  VideoFrame(VideoFrame const & tocopy) : QSharedData( tocopy )
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

  RawVideoFrame	frame;
  quint32	nbpixels;
  quint32	nboctets;
};

class	LightVideoFrame
{
public:

  LightVideoFrame()
  {
    m_videoFrame = new VideoFrame;
  };

  LightVideoFrame(LightVideoFrame const & tocopy) : m_videoFrame(tocopy.m_videoFrame)
  {
  };

  LightVideoFrame&	operator=(LightVideoFrame const & tocopy)
  {
    if ( m_videoFrame->frame.octets == NULL )
    {
      if ( tocopy.m_videoFrame->frame.octets != NULL )
      {
	m_videoFrame->frame.octets = new quint8[tocopy.m_videoFrame->nboctets];
	m_videoFrame->nboctets = tocopy.m_videoFrame->nboctets;
	m_videoFrame->nbpixels = tocopy.m_videoFrame->nboctets / Pixel::NbComposantes;
	quint32	i;
	
	for ( i = 0; i < m_videoFrame->nboctets; ++i )
	    m_videoFrame->frame.octets[i] = tocopy.m_videoFrame->frame.octets[i];
      }
    }
    else
    {
      if ( tocopy.m_videoFrame->frame.octets != NULL )
      {
	if ( m_videoFrame->nboctets != tocopy.m_videoFrame->nboctets )
	{
	  qDebug() << __LINE__ << "\n"
		   << "this pixels = " << m_videoFrame->nbpixels
		   << "tocopy.m_videoFrame pixels = " << tocopy.m_videoFrame->nbpixels;
	  delete [] m_videoFrame->frame.octets;
	  m_videoFrame->frame.octets = new quint8[tocopy.m_videoFrame->nboctets];
	  m_videoFrame->nboctets = tocopy.m_videoFrame->nboctets;
	  m_videoFrame->nbpixels = tocopy.m_videoFrame->nboctets / Pixel::NbComposantes;
	}
	quint32	i;
	  
	for ( i = 0; i < m_videoFrame->nboctets; ++i )
	  m_videoFrame->frame.octets[i] = tocopy.m_videoFrame->frame.octets[i];
      }
      else
      {
	delete [] m_videoFrame->frame.octets;
	m_videoFrame->frame.octets = NULL;
	m_videoFrame->nboctets = 0;
	m_videoFrame->nbpixels = 0;
      }
    }
    
    return ( *this );
  };

  LightVideoFrame(quint32 nboctets)
  {
    m_videoFrame = new VideoFrame;
    m_videoFrame->nboctets = nboctets;
    m_videoFrame->nbpixels = nboctets / Pixel::NbComposantes;
    m_videoFrame->frame.octets = new quint8[nboctets];
  };

  LightVideoFrame(quint8* tocopy, quint32 nboctets)
  {
    quint32	i;

    m_videoFrame = new VideoFrame;
    m_videoFrame->nboctets = nboctets;
    m_videoFrame->nbpixels = nboctets / Pixel::NbComposantes;
    m_videoFrame->frame.octets = new quint8[nboctets];

    for ( i = 0; i < m_videoFrame->nboctets; ++i )
      m_videoFrame->frame.octets[i] = tocopy[i];
  };

  ~LightVideoFrame()
  {
  };
  
  VideoFrame const * operator->(void) const
  {
    return ( m_videoFrame.data() );
  };

  VideoFrame const & operator*(void) const
  {
    return ( *m_videoFrame );
  };

  VideoFrame* operator->(void)
  {
    return ( m_videoFrame.data() );
  };

  VideoFrame& operator*(void)
  {
    return ( *m_videoFrame );
  };
  
private:

  QSharedDataPointer<VideoFrame>	m_videoFrame;

};

int main(void)
{
  LightVideoFrame	vf(100);

  qDebug() << vf->nbpixels;
  return (0);
}
