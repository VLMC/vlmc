#ifndef POUETEFFECT_H_
#define POUETEFFECT_H_

class	PouetEffect : public GenericEffect
{
 public:

  // CTOR & DTOR

  PouetEffect();
  ~PouetEffect();

  // RENDER METHOD

  void	render(void);

 private:
  static	quint const		m_nbVideoInputs = 42;
  static	quint const		m_nbVideoOutputs = 42;
  static	char const *	m_videoInputs[m_nbVideoInputs];
  static	char const *	m_videoOutputs[m_nbVIdeoOutputs];
};

#endif // POUETEFFECT_H_
