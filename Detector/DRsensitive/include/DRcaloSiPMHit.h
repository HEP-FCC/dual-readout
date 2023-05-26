#ifndef DRcaloSiPMHit_h
#define DRcaloSiPMHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include "DD4hep/Objects.h"
#include "DD4hep/Segmentations.h"

namespace drc {
  class DRcaloSiPMHit : public G4VHit {
  public:
    typedef std::map<int, int> DRsimTimeStruct;
    typedef std::map<int, int> DRsimWavlenSpectrum;

    DRcaloSiPMHit(float wavSampling, float timeSampling);
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

    void CountWavlenSpectrum(int ibin);
    const DRsimWavlenSpectrum& GetWavlenSpectrum() const { return fWavlenSpectrum; }

    void CountTimeStruct(int ibin);
    const DRsimTimeStruct& GetTimeStruct() const { return fTimeStruct; }

    float GetSamplingTime() { return mTimeSampling; }
    float GetSamplingWavlen() { return mWavSampling; }

    float GetTimeStart() { return fTimeStart; }
    void SetTimeStart(float val) { fTimeStart = val; }

    float GetTimeEnd() { return fTimeEnd; }
    void SetTimeEnd(float val) { fTimeEnd = val; }

    float GetWavlenMax() { return fWavlenMax; }
    void SetWavlenMax(float val) { fWavlenMax = val; }

    float GetWavlenMin() { return fWavlenMin; }
    void SetWavlenMin(float val) { fWavlenMin = val; }

  private:
    dd4hep::DDSegmentation::CellID fSiPMnum;
    unsigned long fPhotons;
    DRsimWavlenSpectrum fWavlenSpectrum;
    DRsimTimeStruct fTimeStruct;
    float mWavSampling;
    float mTimeSampling;
    float fWavlenMax;
    float fWavlenMin;
    float fTimeStart;
    float fTimeEnd;
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
