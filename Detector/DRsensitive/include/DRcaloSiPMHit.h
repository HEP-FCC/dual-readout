#ifndef DRcaloSiPMHit_h
#define DRcaloSiPMHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include "DD4hep/Objects.h"
#include "DD4hep/Segmentations.h"

namespace ddDRcalo {
  class DRcaloSiPMHit : public G4VHit {
  public:
    typedef std::pair<float,float> hitRange;
    typedef std::map<hitRange, int> DRsimTimeStruct;
    typedef std::map<hitRange, int> DRsimWavlenSpectrum;

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

    void SetSiPMnum(dd4hep::DDSegmentation::CellID n) { fSiPMnum = n; }
    const dd4hep::DDSegmentation::CellID& GetSiPMnum() const { return fSiPMnum; }

    void CountWavlenSpectrum(hitRange range);
    const DRsimWavlenSpectrum& GetWavlenSpectrum() const { return fWavlenSpectrum; }

    void CountTimeStruct(hitRange range);
    const DRsimTimeStruct& GetTimeStruct() const { return fTimeStruct; }

  private:
    dd4hep::DDSegmentation::CellID fSiPMnum;
    unsigned long fPhotons;
    DRsimWavlenSpectrum fWavlenSpectrum;
    DRsimTimeStruct fTimeStruct;
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
