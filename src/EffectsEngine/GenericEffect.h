#ifndef GENERICEFFECT_H_
#define GENERICEFFECT_H_

#include <QtGlobal>
#include <QString>
#include <QHash>

#include "InSlot.hpp"
#include "OutSlot.hpp"
#include "LightVideoFrame.h"

class	GenericEffect
{

 public:
 
 // CTOR & DTOR

  virtual ~GenericEffect();

 protected:

/*   GenericEffect( bool dynamicsvideoinputs, */
/* 		 char const * videoinputs[], */
/* 		 quint32 const nbvideoinputs, */
/* 		 bool dynamicsvideooutputs, */
/* 		 char const * videooutputs[], */
/* 		 quint32 const nbvideooutputs ); */

  GenericEffect( char const * videoinputs[],
		 quint32 const nbvideoinputs,
		 char const * videooutputs[],
		 quint32 const nbvideooutputs );

 public:

  // RENDER METHOD

  virtual void  render(void) = 0;

  // CONNECTION METHODS


 public:

  //
  // CONNECTIONS BETWEEN GENERICEFFECTS
  //

/*   void				connectDynOutToStatIn( GenericEffect* destinationeffect, QString const & inslotname ); */
/*   void				connectDynOutToDynIn( GenericEffect* destinationeffect ); */
  void					connectOutput( quint32 outIndex, GenericEffect* destEffect, quint32 inIndex );
/*   void				connectStatOutToDynIn( QString const & outslotname, GenericEffect* destinationeffect ); */

  //
  // CONNECTIONS BETWEEN GENERICEFFECT & OUTSLOTS/INSLOTS
  //

  void					connect( OutSlot<LightVideoFrame> & out, quint32 inIndex );
  void					connect( quint32 outIndex, InSlot<LightVideoFrame> & in);

 private:

  //

/*   bool				areThereDynamicsVideoInputs( void ); */
/*   bool				areThereDynamicsVideoOutputs( void ); */
/*   InSlot<LightVideoFrame>&	getStaticVideoInput( QString const & name ); */
/*   OutSlot<LightVideoFrame>&	getStaticVideoOutput( QString const & name ); */
/*   InSlot<LightVideoFrame>&	getDynamicVideoInput( void ); */
/*   OutSlot<LightVideoFrame>&	getDynamicVideoOutput( void ); */

 private:

/*   bool						m_dynamicsVideoInputs; */
/*   bool						m_dynamicsVideoOutputs; */

 protected:

  //  QString					m_name;
  InSlot<LightVideoFrame>*			m_videoInputs;
  OutSlot<LightVideoFrame>*			m_videoOutputs;
/*   QHash< QString, InSlot<SoundSampleFrame> >	m_soundInputs; */
/*   QHash< QString, OutSlot<SoundSampleFrame> >m_soundOutputs; */
/*   QHash< QString, InSlot<LightVideoFrame> >	m_controlInputs; */
/*   QHash< QString, OutSlot<LightVideoFrame> >	m_controlOutputs; */
  
};

#endif // GENERICEFFECT_H_
