#include "DRsimSiPMHit.hh"

G4ThreadLocal G4Allocator<DRsimSiPMHit>* DRsimSiPMHitAllocator = 0;

DRsimSiPMHit::DRsimSiPMHit(G4int wavBin, G4int timeBin)
: G4VHit(),
  fSiPMnum(0),
  fPhotons(0),
  fTowerTheta(std::make_pair(999,999.)),
  fTowerPhi(std::make_pair(999,999.)),
  fTowerXY(std::make_pair(-1,-1)),
  fSiPMXY(std::make_pair(-1,-1)),
  fWavBin(wavBin),
  fTimeBin(timeBin)
{}

DRsimSiPMHit::~DRsimSiPMHit() {}

DRsimSiPMHit::DRsimSiPMHit(const DRsimSiPMHit &right)
: G4VHit() {
  fSiPMnum = right.fSiPMnum;
  fPhotons = right.fPhotons;
  fTowerTheta = right.fTowerTheta;
  fTowerPhi = right.fTowerPhi;
  fTowerXY = right.fTowerXY;
  fSiPMXY = right.fSiPMXY;
  fWavelen = right.fWavelen;
  fTimeCount = right.fTimeCount;
}

const DRsimSiPMHit& DRsimSiPMHit::operator=(const DRsimSiPMHit &right) {
  fSiPMnum = right.fSiPMnum;
  fPhotons = right.fPhotons;
  fTowerTheta = right.fTowerTheta;
  fTowerPhi = right.fTowerPhi;
  fTowerXY = right.fTowerXY;
  fSiPMXY = right.fSiPMXY;
  fWavelen = right.fWavelen;
  fTimeCount = right.fTimeCount;
  return *this;
}

G4int DRsimSiPMHit::operator==(const DRsimSiPMHit &right) const {
  return (fSiPMnum==right.fSiPMnum && fTowerTheta==right.fTowerTheta && fTowerPhi==right.fTowerPhi && fSiPMXY==right.fSiPMXY);
}

void DRsimSiPMHit::Draw() {
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager) {}
}

void DRsimSiPMHit::Print() {}

void CountWavlenSpectrum(hitRange range) {
  auto it = fWavlenSpectrum.find(range);
  if (it==fWavlenSpectrum.end()) fWavlenSpectrum.insert(std::make_pair(range,1));
  else it->second++;
}

void CountTimeStruct(hitRange range) {
  auto it = fTimeStruct.find(range);
  if (it==fTimeStruct.end()) fTimeStruct.insert(std::make_pair(range,1));
  else it->second++;
}
