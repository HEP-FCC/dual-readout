#ifndef DRsimPMTSD_h
#define DRsimPMTSD_h 1

#include "globals.hh"
#include "G4VSensitiveDetector.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

#include "DRsimPMTHit.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class DRsimPMTSD : public G4VSensitiveDetector {
public:
  DRsimPMTSD(const G4String& name, const G4String& hitsCollectionName);
  virtual ~DRsimPMTSD();

  virtual void Initialize(G4HCofThisEvent* HCE);
  virtual G4bool ProcessHits(G4Step* aStep,G4TouchableHistory*);
  virtual void EndOfEvent(G4HCofThisEvent* HCE);

private:
  DRsimPMTHitsCollection* fHitCollection;
  G4int fHCID;
  G4int fSDID;
  bool fIsR;
  bool fIsB;
  G4int fWavBin;
  G4int fTimeBin;

  G4double wavToE(G4double wav) { return h_Planck*c_light/wav; }

  G4int findBin(G4double en) {
    for (int i = 0; i < fWavBin+1; i++) {
      if ( en < wavToE( 900*nm - i*600*nm/(double)fWavBin ) ) return i;
      else continue;
    }
    return fWavBin+1;
  }

  G4int findTimeBin(G4double stepTime) {
    for (int i = 0; i < fTimeBin+1; i++) {
      if ( stepTime < ( 10*ns + i*60*ns/(double)fTimeBin ) ) return i;
      else continue;
    }
    return fTimeBin+1;
  }
};

#endif
