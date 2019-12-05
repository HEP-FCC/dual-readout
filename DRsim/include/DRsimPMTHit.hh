#ifndef DRsimPMTHit_h
#define DRsimPMTHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"

class DRsimPMTHit : public G4VHit {
public:
  DRsimPMTHit(G4int wavBin, G4int timeBin);
  DRsimPMTHit(const DRsimPMTHit &right);
  virtual ~DRsimPMTHit();

  const DRsimPMTHit& operator=(const DRsimPMTHit &right);
  int operator==(const DRsimPMTHit &right) const;

  void *operator new(size_t);
  void operator delete(void* aHit);

  void Draw();
  void Print();

  void photonCount() { fPhotons++; }
  G4int GetPhotonCount() const { return fPhotons; }

  void SetPMTnum(G4int n) { fPMTnum = n; }
  G4int GetPMTnum() const { return fPMTnum; }

  void SetTowerIEta(G4int eta) { fTowerIEta = eta; }
  G4int GetTowerIEta() const { return fTowerIEta; }

  void SetTowerIPhi(G4int phi) { fTowerIPhi = phi; }
  G4int GetTowerIPhi() const { return fTowerIPhi; }

  void wavelenCount(G4int bin) { fWavelen.at(bin)++; }
  G4int GetWavelenCount(G4int bin) const { return fWavelen.at(bin); }

  void timeCount(G4int bin) { fTimeCount.at(bin)++; }
  G4int GetTimeCount(G4int bin) const { return fTimeCount.at(bin); }

private:
  G4int fPMTnum;
  G4int fPhotons;
  G4int fTowerIEta;
  G4int fTowerIPhi;
  std::vector<G4int> fWavelen;
  std::vector<G4int> fTimeCount;
  G4int fWavBin;
  G4int fTimeBin;
};

typedef G4THitsCollection<DRsimPMTHit> DRsimPMTHitsCollection;
extern G4ThreadLocal G4Allocator<DRsimPMTHit>* DRsimPMTHitAllocator;

#endif
