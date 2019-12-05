#include "DRsimPMTHit.hh"

#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include <iomanip>

G4ThreadLocal G4Allocator<DRsimPMTHit>* DRsimPMTHitAllocator = 0;

DRsimPMTHit::DRsimPMTHit(G4int wavBin, G4int timeBin)
: G4VHit(),
  fPMTnum(0),
  fPhotons(0),
  fTowerIEta(999),
  fTowerIPhi(999),
  fWavelen(wavBin+2,0),
  fTimeCount(timeBin+2,0),
  fWavBin(wavBin),
  fTimeBin(timeBin)
{}

DRsimPMTHit::~DRsimPMTHit() {}

DRsimPMTHit::DRsimPMTHit(const DRsimPMTHit &right)
: G4VHit() {
  fPMTnum = right.fPMTnum;
  fPhotons = right.fPhotons;
  fTowerIEta = right.fTowerIEta;
  fTowerIPhi = right.fTowerIPhi;
  fWavelen = right.fWavelen;
  fTimeCount = right.fTimeCount;
}

const DRsimPMTHit& DRsimPMTHit::operator=(const DRsimPMTHit &right) {
  fPMTnum = right.fPMTnum;
  fPhotons = right.fPhotons;
  fTowerIEta = right.fTowerIEta;
  fTowerIPhi = right.fTowerIPhi;
  fWavelen = right.fWavelen;
  fTimeCount = right.fTimeCount;
  return *this;
}

G4int DRsimPMTHit::operator==(const DRsimPMTHit &right) const {
  return (fPMTnum==right.fPMTnum && fTowerIEta==right.fTowerIEta && fTowerIPhi==right.fTowerIPhi);
}

void* DRsimPMTHit::operator new(size_t) {
  if (!DRsimPMTHitAllocator) DRsimPMTHitAllocator = new G4Allocator<DRsimPMTHit>;
  return (void*)DRsimPMTHitAllocator->MallocSingle();
}

void DRsimPMTHit::operator delete(void*aHit) {
  DRsimPMTHitAllocator->FreeSingle((DRsimPMTHit*) aHit);
}

void DRsimPMTHit::Draw() {
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager) {}
}

void DRsimPMTHit::Print() {}
