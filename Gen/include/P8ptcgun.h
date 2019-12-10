#ifndef P8ptcgun_h
#define P8ptcgun_h 1

#include "Pythia8/Pythia.h"

using namespace Pythia8;

class P8ptcgun {
public:
  P8ptcgun(int id, double ee, double thetaIn, double phiIn);
  ~P8ptcgun();

  void fillResonance(Event& event, ParticleData& pdt, Rndm& rndm, bool atRest);
  void fillParton(Event& event, ParticleData& pdt, Rndm& rndm, bool atRest, double scale);

private:
  int fId;
  double fE;
  double fThetaIn;
  double fPhiIn;
};

#endif
