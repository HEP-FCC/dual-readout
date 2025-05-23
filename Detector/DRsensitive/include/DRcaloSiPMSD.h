#ifndef DRcaloSiPMSD_h
#define DRcaloSiPMSD_h 1

#include "DD4hep/Segmentations.h"
#include "DRcaloSiPMHit.h"
#include "GridDRcalo.h"

#include "G4PhysicalConstants.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4TouchableHistory.hh"
#include "G4VSensitiveDetector.hh"

namespace drc {
class DRcaloSiPMSD : public G4VSensitiveDetector {
public:
  DRcaloSiPMSD(const std::string aName, const std::string aReadoutName, const dd4hep::Segmentation& aSeg);
  ~DRcaloSiPMSD();

  virtual void Initialize(G4HCofThisEvent* HCE) final;
  virtual bool ProcessHits(G4Step* aStep, G4TouchableHistory*) final;

private:
  DRcaloSiPMHitsCollection* fHitCollection;
  dd4hep::DDSegmentation::GridDRcalo* fSeg;
  G4int fHCID;

  G4int fWavBin;
  G4int fTimeBin;
  G4float fWavlenStart;
  G4float fWavlenEnd;
  G4float fTimeStart;
  G4float fTimeEnd;
  G4float fWavlenStep;
  G4float fTimeStep;

  G4double wavToE(G4double wav) { return h_Planck * c_light / wav; }

  int findWavBin(G4double en);
  int findTimeBin(G4double stepTime);
};
} // namespace drc

#endif
