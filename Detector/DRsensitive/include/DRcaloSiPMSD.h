#ifndef DRcaloSiPMSD_h
#define DRcaloSiPMSD_h 1

#include "DRcaloSiPMHit.h"
#include "DRsimInterface.h"
#include "GridDRcalo.h"
#include "DD4hep/Segmentations.h"

#include "G4VSensitiveDetector.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"

namespace ddDRcalo {
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

    G4double wavToE(G4double wav) { return h_Planck*c_light/wav; }

    DRsimInterface::hitRange findWavRange(G4double en);
    DRsimInterface::hitRange findTimeRange(G4double stepTime);
  };
}

#endif
