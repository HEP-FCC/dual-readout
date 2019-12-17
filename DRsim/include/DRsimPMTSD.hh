#ifndef DRsimSiPMSD_h
#define DRsimSiPMSD_h 1

#include "DRsimSiPMHit.hh"

#include "G4VSensitiveDetector.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"

class DRsimSiPMSD : public G4VSensitiveDetector {
public:
  DRsimSiPMSD(const G4String& name, const G4String& hitsCollectionName, std::pair<int,float> towerTheta, DRsimSiPMHit::hitXY towerXY);
  virtual ~DRsimSiPMSD();

  virtual void Initialize(G4HCofThisEvent* HCE);
  virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory*);
  virtual void EndOfEvent(G4HCofThisEvent* HCE);

private:
  DRsimSiPMHitsCollection* fHitCollection;
  G4int fHCID;
  G4int fWavBin;
  G4int fTimeBin;
  G4float fWavlenStart;
  G4float fWavlenEnd;
  G4float fWavlenStep;
  G4float fTimeStart;
  G4float fTimeEnd;
  G4float fTimeStep;
  G4float fPhiUnit;
  std::pair<int,float> fTowerTheta;
  DRsimSiPMHit::hitXY fTowerXY;

  G4double wavToE(G4double wav) { return h_Planck*c_light/wav; }

  DRsimSiPMHit::hitRange findWavRange(G4double en);
  DRsimSiPMHit::hitRange findTimeRange(G4double stepTime);
  DRsimSiPMHit::hitXY findSiPMXY(G4int SiPMnum, DRsimSiPMHit::hitXY towerXY);
};

#endif
