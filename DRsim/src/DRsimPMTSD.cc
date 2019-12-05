#include "DRsimPMTSD.hh"
#include "DRsimPMTHit.hh"

#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
using namespace std;

DRsimPMTSD::DRsimPMTSD(const G4String& name, const G4String& hitsCollectionName)
: G4VSensitiveDetector(name), fHitCollection(0), fHCID(-1), fWavBin(60), fTimeBin(300)
{
  fSDID = std::stoi(name.substr(2));
  fIsR = (name.find("R")==std::string::npos) ? false : true;
  fIsB = (name.find("B")==std::string::npos) ? false : true;
  collectionName.insert(hitsCollectionName);
  if(!fIsB) fSDID += 52;
  if(!fIsR) fSDID = -fSDID -1;
}

DRsimPMTSD::~DRsimPMTSD() {}

void DRsimPMTSD::Initialize(G4HCofThisEvent* hce) {
  fHitCollection = new DRsimPMTHitsCollection(SensitiveDetectorName,collectionName[0]);
  if (fHCID<0) { fHCID = GetCollectionID(0); }
  hce->AddHitsCollection(fHCID,fHitCollection);
}

G4bool DRsimPMTSD::ProcessHits(G4Step* step, G4TouchableHistory*) {
  if(step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false;

  G4int PMTnum = step->GetPostStepPoint()->GetTouchable()->GetVolume(1)->GetCopyNo();
  G4int towerPhi = step->GetPostStepPoint()->GetTouchable()->GetVolume(3)->GetCopyNo();
  G4int nofHits = fHitCollection->entries();
  G4double hitTime = step->GetPostStepPoint()->GetGlobalTime();
  G4double energy = step->GetTrack()->GetTotalEnergy();
  DRsimPMTHit* hit = NULL;

  for (G4int i = 0; i < nofHits; i++) {
    if ( (*fHitCollection)[i]->GetPMTnum()==PMTnum && (*fHitCollection)[i]->GetTowerIEta()==fSDID && (*fHitCollection)[i]->GetTowerIPhi()==towerPhi) {
      hit = (*fHitCollection)[i];
      break;
    }
  }

  if (hit==NULL) {
    hit = new DRsimPMTHit(fWavBin,fTimeBin);
    hit->SetPMTnum(PMTnum);
    hit->SetTowerIEta(fSDID);
    hit->SetTowerIPhi(towerPhi);
    fHitCollection->insert(hit);
  }

  G4int bin = findBin(energy);
  hit->photonCount();
  hit->wavelenCount(bin);

  G4int timeBin = findTimeBin(hitTime);
  hit->timeCount(timeBin);

  return true;
}

void DRsimPMTSD::EndOfEvent(G4HCofThisEvent*) {
  if ( verboseLevel>1 ) {
    G4int nofHits = fHitCollection->entries();
    G4cout
    << G4endl
    << "-------->Hits Collection: in this event they are " << nofHits
    << " hits in the tracker chambers: " << G4endl;
    for ( G4int i=0; i<nofHits; i++ ) (*fHitCollection)[i]->Print();
  }
}
