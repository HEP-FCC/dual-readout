#ifndef DRsimSiPMSD_h
#define DRsimSiPMSD_h 1

#include "DRsimSiPMHit.hh"
#include "DRsimInterface.h"

#include "G4VSensitiveDetector.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"

class DRsimSiPMSD : public G4VSensitiveDetector {
public:
  DRsimSiPMSD(const G4String& name, const G4String& hitsCollectionName, DRsimInterface::DRsimTowerProperty towerProp);
  virtual ~DRsimSiPMSD();

  virtual void Initialize(G4HCofThisEvent* HCE);
  virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory*);
  virtual void EndOfEvent(G4HCofThisEvent* HCE);

private:
  DRsimSiPMHitsCollection* fHitCollection;
  G4int fHCID;
  G4int fWavBin;
  G4int fTimeBin;
  G4float fPhiUnit;
  G4float fWavlenStart;
  G4float fWavlenEnd;
  G4float fTimeStart;
  G4float fTimeEnd;
  G4float fWavlenStep;
  G4float fTimeStep;
  std::pair<int,float> fTowerTheta;
  DRsimInterface::hitXY fTowerXY;
  G4float fInnerR;
  G4float fTowerH;
  G4float fDTheta;

  G4double wavToE(G4double wav) { return h_Planck*c_light/wav; }

  DRsimInterface::hitRange findWavRange(G4double en);
  DRsimInterface::hitRange findTimeRange(G4double stepTime);
  DRsimInterface::hitXY findSiPMXY(G4int SiPMnum, DRsimInterface::hitXY towerXY);
};

#endif
