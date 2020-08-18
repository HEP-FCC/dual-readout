#include "DRcaloSiPMHit.h"

G4ThreadLocal G4Allocator<ddDRcalo::DRcaloSiPMHit>* ddDRcalo::DRcaloSiPMHitAllocator = 0;

ddDRcalo::DRcaloSiPMHit::DRcaloSiPMHit(G4int wavBin, G4int timeBin)
: G4VHit(),
  fSiPMnum(0),
  fPhotons(0),
  fWavBin(wavBin),
  fTimeBin(timeBin)
{}

ddDRcalo::DRcaloSiPMHit::~DRcaloSiPMHit() {}

ddDRcalo::DRcaloSiPMHit::DRcaloSiPMHit(const ddDRcalo::DRcaloSiPMHit &right)
: G4VHit() {
  fSiPMnum = right.fSiPMnum;
  fPhotons = right.fPhotons;
  fWavlenSpectrum = right.fWavlenSpectrum;
  fTimeStruct = right.fTimeStruct;
}

const ddDRcalo::DRcaloSiPMHit& ddDRcalo::DRcaloSiPMHit::operator=(const ddDRcalo::DRcaloSiPMHit &right) {
  fSiPMnum = right.fSiPMnum;
  fPhotons = right.fPhotons;
  fWavlenSpectrum = right.fWavlenSpectrum;
  fTimeStruct = right.fTimeStruct;
  return *this;
}

G4bool ddDRcalo::DRcaloSiPMHit::operator==(const ddDRcalo::DRcaloSiPMHit &right) const {
  return (fSiPMnum==right.fSiPMnum);
}

void ddDRcalo::DRcaloSiPMHit::CountWavlenSpectrum(DRsimInterface::hitRange range) {
  auto it = fWavlenSpectrum.find(range);
  if (it==fWavlenSpectrum.end()) fWavlenSpectrum.insert(std::make_pair(range,1));
  else it->second++;
}

void ddDRcalo::DRcaloSiPMHit::CountTimeStruct(DRsimInterface::hitRange range) {
  auto it = fTimeStruct.find(range);
  if (it==fTimeStruct.end()) fTimeStruct.insert(std::make_pair(range,1));
  else it->second++;
}
