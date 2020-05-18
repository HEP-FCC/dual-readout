#include "P8filter.h"

P8filter::P8filter(bool on, double etaMax, double thres) {
  fOn = on;
  fEta = etaMax;
  fThres = thres;
}

P8filter::~P8filter() {}

void P8filter::init() {
  fSum = 0.;
}

bool P8filter::filter(Pythia8::Event& evt, Pythia8::Info& info) {
  if (!fOn) return true;

  init();

  double absThres = fThres*info.eCM();

  for (int i = 0; i < evt.size(); ++i) {
    // Final state only
    if (!evt[i].isFinal())        continue;

    // No neutrinos
    if (evt[i].idAbs() == 12 || evt[i].idAbs() == 14 ||
        evt[i].idAbs() == 16)     continue;

    if ( std::abs(evt[i].eta()) > fEta ) {
      fSum += evt[i].e();
    }

    if ( absThres < fSum ) return false;
  }

  return true;
}
