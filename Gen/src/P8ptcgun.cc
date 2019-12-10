#include "P8ptcgun.h"

P8ptcgun::P8ptcgun(int id, double ee, double thetaIn, double phiIn)
: fId(id), fE(ee), fThetaIn(thetaIn), fPhiIn(phiIn) {}

P8ptcgun::~P8ptcgun() {}

void P8ptcgun::fillResonance(Event& event, ParticleData& pdt, Rndm& rndm, bool atRest = false) {

  // Reset event record to allow for new event.
  event.reset();

  // Select particle mass; where relevant according to Breit-Wigner.
  double mm = pdt.mSel(fId);
  double pp = sqrtpos(fE*fE - mm*mm);

  // Special case when particle is supposed to be at rest.
  if (atRest) {
    fE = mm;
    pp = 0.;
  }

  // Angles as input or uniform in solid angle.
  double cThe, sThe, phi;
  cThe = cos(fThetaIn);
  sThe = sin(fThetaIn);
  phi  = fPhiIn;

  // Store the particle in the event record.
  event.append( fId, 1, 0, 0, pp * sThe * cos(phi), pp * sThe * sin(phi), pp * cThe, fE, mm);
}

void P8ptcgun::fillParton(Event& event, ParticleData& pdt, Rndm& rndm, bool atRest = false, double scale = 20.) {

  // Reset event record to allow for new event.
  event.reset();

  // Select particle mass; where relevant according to Breit-Wigner.
  double mm = pdt.mSel(fId);
  double pp = sqrtpos(fE*fE - mm*mm);

  // Special case when particle is supposed to be at rest.
  if (atRest) {
    fE = mm;
    pp = 0.;
  }

  // Angles as input or uniform in solid angle.
  double cThe, sThe, phi;
  cThe = cos(fThetaIn);
  sThe = sin(fThetaIn);
  phi  = fPhiIn;

  int col1, acol1, col2, acol2, aid;
  if (fId==21) { col1 = 101; acol1 = 102; col2 = 102; acol2 = 101; aid = fId; }
  else { col1 = 101; acol1 = 0; col2 = 0; acol2 = 101; aid = -fId; }

  // Store the particle in the event record.
  event.append( fId, 23, col1, acol1, pp * sThe * cos(phi), pp * sThe * sin(phi), pp * cThe, fE, mm, scale);
  event.append( aid, 23, col2, acol2, -pp * sThe * cos(phi), -pp * sThe * sin(phi), -pp * cThe, fE, mm, scale);
}
