#ifndef DRcaloSiPMHit_h
#define DRcaloSiPMHit_h 1

#include "DRsimInterface.h"

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include "DD4hep/Objects.h"

namespace ddDRcalo {
  class DRcaloSiPMHit : public G4VHit {
  public:

    DRcaloSiPMHit(G4int wavBin, G4int timeBin);
    DRcaloSiPMHit(const DRcaloSiPMHit &right);
    virtual ~DRcaloSiPMHit();

    const DRcaloSiPMHit& operator=(const DRcaloSiPMHit &right);
    G4bool operator==(const DRcaloSiPMHit &right) const;

    inline void *operator new(size_t);
    inline void operator delete(void* aHit);

    virtual void Draw() {};
    virtual void Print() {};

    void photonCount() { fPhotons++; }
    unsigned long GetPhotonCount() const { return fPhotons; }

    void SetSiPMnum(uint64_t n) { fSiPMnum = n; }
    uint64_t GetSiPMnum() const { return fSiPMnum; }

    void SetSiPMpos(dd4hep::Position pos) { fSiPMpos = pos; }
    dd4hep::Position GetSiPMpos() const { return fSiPMpos; }

    void CountWavlenSpectrum(DRsimInterface::hitRange range);
    DRsimInterface::DRsimWavlenSpectrum GetWavlenSpectrum() const { return fWavlenSpectrum; }

    void CountTimeStruct(DRsimInterface::hitRange range);
    DRsimInterface::DRsimTimeStruct GetTimeStruct() const { return fTimeStruct; }

  private:
    uint64_t fSiPMnum;
    unsigned long fPhotons;
    dd4hep::Position fSiPMpos;
    DRsimInterface::DRsimWavlenSpectrum fWavlenSpectrum;
    DRsimInterface::DRsimTimeStruct fTimeStruct;
    G4int fWavBin;
    G4int fTimeBin;
  };

  typedef G4THitsCollection<DRcaloSiPMHit> DRcaloSiPMHitsCollection;
  extern G4ThreadLocal G4Allocator<DRcaloSiPMHit>* DRcaloSiPMHitAllocator;

  inline void* DRcaloSiPMHit::operator new(size_t) {
    if (!DRcaloSiPMHitAllocator) DRcaloSiPMHitAllocator = new G4Allocator<DRcaloSiPMHit>;
    return (void*)DRcaloSiPMHitAllocator->MallocSingle();
  }

  inline void DRcaloSiPMHit::operator delete(void*aHit) {
    DRcaloSiPMHitAllocator->FreeSingle((DRcaloSiPMHit*) aHit);
  }
}

#endif
