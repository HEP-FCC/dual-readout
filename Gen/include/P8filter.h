#ifndef P8filter_h
#define P8filter_h

#include "Pythia8/Pythia.h"

class P8filter {
public:
  P8filter(bool on, double etaMax, double thres);
  ~P8filter();

  bool filter(Pythia8::Event& evt, Pythia8::Info& info);

private:
  void init();
  
  bool fOn;
  double fEta;
  double fThres;
  double fSum;
};

#endif
