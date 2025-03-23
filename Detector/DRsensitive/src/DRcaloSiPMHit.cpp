#include "DRcaloSiPMHit.h"

G4ThreadLocal G4Allocator<drc::DRcaloSiPMHit>* drc::DRcaloSiPMHitAllocator = 0;

drc::DRcaloSiPMHit::DRcaloSiPMHit(float wavSampling, float timeSampling)
    : G4VHit(), fSiPMnum(0), fPhotons(0), mWavSampling(wavSampling), mTimeSampling(timeSampling) {}

drc::DRcaloSiPMHit::~DRcaloSiPMHit() {}

drc::DRcaloSiPMHit::DRcaloSiPMHit(const drc::DRcaloSiPMHit& right) : G4VHit() {
  fSiPMnum = right.fSiPMnum;
  fPhotons = right.fPhotons;
  fWavlenSpectrum = right.fWavlenSpectrum;
  fTimeStruct = right.fTimeStruct;
  mWavSampling = right.mWavSampling;
  mTimeSampling = right.mTimeSampling;
}

const drc::DRcaloSiPMHit& drc::DRcaloSiPMHit::operator=(const drc::DRcaloSiPMHit& right) {
  fSiPMnum = right.fSiPMnum;
  fPhotons = right.fPhotons;
  fWavlenSpectrum = right.fWavlenSpectrum;
  fTimeStruct = right.fTimeStruct;
  mWavSampling = right.mWavSampling;
  mTimeSampling = right.mTimeSampling;
  return *this;
}

G4bool drc::DRcaloSiPMHit::operator==(const drc::DRcaloSiPMHit& right) const { return (fSiPMnum == right.fSiPMnum); }

void drc::DRcaloSiPMHit::CountWavlenSpectrum(int ibin) {
  auto it = fWavlenSpectrum.find(ibin);

  if (it == fWavlenSpectrum.end())
    fWavlenSpectrum.insert(std::make_pair(ibin, 1));
  else
    it->second++;
}

void drc::DRcaloSiPMHit::CountTimeStruct(int ibin) {
  auto it = fTimeStruct.find(ibin);

  if (it == fTimeStruct.end())
    fTimeStruct.insert(std::make_pair(ibin, 1));
  else
    it->second++;
}
