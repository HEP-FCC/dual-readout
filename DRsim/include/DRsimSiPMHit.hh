#ifndef DRsimSiPMHit_h
#define DRsimSiPMHit_h 1

#include "DRsimInterface.h"

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class DRsimSiPMHit : public G4VHit {
public:

  DRsimSiPMHit(G4int wavBin, G4int timeBin);
  DRsimSiPMHit(const DRsimSiPMHit &right);
  virtual ~DRsimSiPMHit();

  const DRsimSiPMHit& operator=(const DRsimSiPMHit &right);
  G4bool operator==(const DRsimSiPMHit &right) const;

  inline void *operator new(size_t);
  inline void operator delete(void* aHit);

  void Draw();
  void Print();

  void photonCount() { fPhotons++; }
  G4int GetPhotonCount() const { return fPhotons; }

  void SetSiPMnum(G4int n) { fSiPMnum = n; }
  G4int GetSiPMnum() const { return fSiPMnum; }

  void SetSiPMpos(G4ThreeVector pos) { fSiPMpos = pos; }
  G4ThreeVector GetSiPMpos() const { return fSiPMpos; }

  void SetTowerTheta(std::pair<int,float> theta) { fTowerTheta = theta; }
  std::pair<int,float> GetTowerTheta() const { return fTowerTheta; }

  void SetTowerPhi(std::pair<int,float> phi) { fTowerPhi = phi; }
  std::pair<int,float> GetTowerPhi() const { return fTowerPhi; }

  void SetTowerXY(DRsimInterface::hitXY xy) { fTowerXY = xy; }
  DRsimInterface::hitXY GetTowerXY() const { return fTowerXY; }

  void SetTowerInnerR(G4float innerR) { fInnerR = innerR; }
  G4float GetTowerInnerR() const { return fInnerR; }

  void SetTowerH(G4float towerH) { fTowerH = towerH; }
  G4float GetTowerH() const { return fTowerH; }

  void SetTowerDTheta(G4float dTheta) { fDTheta = dTheta; }
  G4float GetTowerDTheta() const { return fDTheta; }

  void SetSiPMXY(DRsimInterface::hitXY xy) { fSiPMXY = xy; }
  DRsimInterface::hitXY GetSiPMXY() const { return fSiPMXY; }

  void CountWavlenSpectrum(DRsimInterface::hitRange range);
  DRsimInterface::DRsimWavlenSpectrum GetWavlenSpectrum() const { return fWavlenSpectrum; }

  void CountTimeStruct(DRsimInterface::hitRange range);
  DRsimInterface::DRsimTimeStruct GetTimeStruct() const { return fTimeStruct; }

private:
  G4int fSiPMnum;
  G4int fPhotons;
  G4ThreeVector fSiPMpos;
  std::pair<int,float> fTowerTheta;
  std::pair<int,float> fTowerPhi;
  DRsimInterface::hitXY fTowerXY;
  G4float fInnerR;
  G4float fTowerH;
  G4float fDTheta;
  DRsimInterface::hitXY fSiPMXY;
  DRsimInterface::DRsimWavlenSpectrum fWavlenSpectrum;
  DRsimInterface::DRsimTimeStruct fTimeStruct;

  G4int fWavBin;
  G4int fTimeBin;
};

typedef G4THitsCollection<DRsimSiPMHit> DRsimSiPMHitsCollection;
extern G4ThreadLocal G4Allocator<DRsimSiPMHit>* DRsimSiPMHitAllocator;

inline void* DRsimSiPMHit::operator new(size_t) {
  if (!DRsimSiPMHitAllocator) DRsimSiPMHitAllocator = new G4Allocator<DRsimSiPMHit>;
  return (void*)DRsimSiPMHitAllocator->MallocSingle();
}

inline void DRsimSiPMHit::operator delete(void*aHit) {
  DRsimSiPMHitAllocator->FreeSingle((DRsimSiPMHit*) aHit);
}

#endif
