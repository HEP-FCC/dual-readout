#include "DRsimSiPMHit.hh"

G4ThreadLocal G4Allocator<DRsimSiPMHit>* DRsimSiPMHitAllocator = 0;

DRsimSiPMHit::DRsimSiPMHit(G4int wavBin, G4int timeBin)
: G4VHit(),
  fSiPMnum(0),
  fPhotons(0),
  fSiPMpos(G4ThreeVector(0.,0.,0.)),
  fTowerTheta(std::make_pair(999,999.)),
  fTowerPhi(std::make_pair(999,999.)),
  fTowerXY(std::make_pair(-1,-1)),
  fInnerR(0.),
  fTowerH(0.),
  fDTheta(0.),
  fSiPMXY(std::make_pair(-1,-1)),
  fWavBin(wavBin),
  fTimeBin(timeBin)
{}

DRsimSiPMHit::~DRsimSiPMHit() {}

DRsimSiPMHit::DRsimSiPMHit(const DRsimSiPMHit &right)
: G4VHit() {
  fSiPMnum = right.fSiPMnum;
  fPhotons = right.fPhotons;
  fSiPMpos = right.fSiPMpos;
  fTowerTheta = right.fTowerTheta;
  fTowerPhi = right.fTowerPhi;
  fTowerXY = right.fTowerXY;
  fInnerR = right.fInnerR;
  fTowerH = right.fTowerH;
  fDTheta = right.fDTheta;
  fSiPMXY = right.fSiPMXY;
  fWavlenSpectrum = right.fWavlenSpectrum;
  fTimeStruct = right.fTimeStruct;
}

const DRsimSiPMHit& DRsimSiPMHit::operator=(const DRsimSiPMHit &right) {
  fSiPMnum = right.fSiPMnum;
  fPhotons = right.fPhotons;
  fSiPMpos = right.fSiPMpos;
  fTowerTheta = right.fTowerTheta;
  fTowerPhi = right.fTowerPhi;
  fTowerXY = right.fTowerXY;
  fInnerR = right.fInnerR;
  fTowerH = right.fTowerH;
  fDTheta = right.fDTheta;
  fSiPMXY = right.fSiPMXY;
  fWavlenSpectrum = right.fWavlenSpectrum;
  fTimeStruct = right.fTimeStruct;
  return *this;
}

G4bool DRsimSiPMHit::operator==(const DRsimSiPMHit &right) const {
  return (fSiPMnum==right.fSiPMnum && fTowerTheta==right.fTowerTheta && fTowerPhi==right.fTowerPhi && fSiPMXY==right.fSiPMXY);
}

void DRsimSiPMHit::Draw() {}

void DRsimSiPMHit::Print() {}

void DRsimSiPMHit::CountWavlenSpectrum(DRsimInterface::hitRange range) {
  auto it = fWavlenSpectrum.find(range);
  if (it==fWavlenSpectrum.end()) fWavlenSpectrum.insert(std::make_pair(range,1));
  else it->second++;
}

void DRsimSiPMHit::CountTimeStruct(DRsimInterface::hitRange range) {
  auto it = fTimeStruct.find(range);
  if (it==fTimeStruct.end()) fTimeStruct.insert(std::make_pair(range,1));
  else it->second++;
}
