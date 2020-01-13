#include "DRsimSiPMSD.hh"
#include "DRsimSiPMHit.hh"
#include "DRsimDetectorConstruction.hh"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

using namespace std;

DRsimSiPMSD::DRsimSiPMSD(const G4String& name, const G4String& hitsCollectionName, DRsimInterface::DRsimTowerProperty towerProp)
: G4VSensitiveDetector(name), fHitCollection(0), fHCID(-1), fWavBin(60), fTimeBin(600),
fPhiUnit(2*M_PI/(G4float)DRsimDetectorConstruction::sNumZRot), fWavlenStart(900.), fWavlenEnd(300.), fTimeStart(10.), fTimeEnd(70.)
{
  collectionName.insert(hitsCollectionName);
  fWavlenStep = (fWavlenStart-fWavlenEnd)/(float)fWavBin;
  fTimeStep = (fTimeEnd-fTimeStart)/(float)fTimeBin;

  fTowerTheta = towerProp.towerTheta;
  fTowerXY = towerProp.towerXY;
  fInnerR = towerProp.innerR;
  fTowerH = towerProp.towerH;
  fDTheta = towerProp.dTheta;
}

DRsimSiPMSD::~DRsimSiPMSD() {}

void DRsimSiPMSD::Initialize(G4HCofThisEvent* hce) {
  fHitCollection = new DRsimSiPMHitsCollection(SensitiveDetectorName,collectionName[0]);
  if (fHCID<0) { fHCID = GetCollectionID(0); }
  hce->AddHitsCollection(fHCID,fHitCollection);
}

G4bool DRsimSiPMSD::ProcessHits(G4Step* step, G4TouchableHistory*) {
  if(step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false;

  G4int SiPMnum = step->GetPostStepPoint()->GetTouchable()->GetVolume(1)->GetCopyNo();
  G4int towerPhi = step->GetPostStepPoint()->GetTouchable()->GetVolume(3)->GetCopyNo();
  G4int nofHits = fHitCollection->entries();
  G4double hitTime = step->GetPostStepPoint()->GetGlobalTime();
  G4double energy = step->GetTrack()->GetTotalEnergy();

  DRsimSiPMHit* hit = NULL;

  for (G4int i = 0; i < nofHits; i++) {
    if ( (*fHitCollection)[i]->GetSiPMnum()==SiPMnum && (*fHitCollection)[i]->GetTowerTheta()==fTowerTheta && (*fHitCollection)[i]->GetTowerPhi().first==towerPhi) {
      hit = (*fHitCollection)[i];
      break;
    }
  }

  if (hit==NULL) {
    hit = new DRsimSiPMHit(fWavBin,fTimeBin);
    hit->SetSiPMnum(SiPMnum);
    hit->SetTowerTheta(fTowerTheta);
    hit->SetTowerPhi(std::make_pair(towerPhi,(float)towerPhi*fPhiUnit));
    hit->SetTowerXY(fTowerXY);
    hit->SetTowerInnerR(fInnerR);
    hit->SetTowerH(fTowerH);
    hit->SetTowerDTheta(fDTheta);
    hit->SetSiPMXY(findSiPMXY(SiPMnum,fTowerXY));
    hit->SetSiPMpos(step->GetPostStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform().Inverse().TransformPoint(G4ThreeVector(0.,0.,0.)));

    fHitCollection->insert(hit);
  }

  hit->photonCount();

  DRsimInterface::hitRange wavRange = findWavRange(energy);
  hit->CountWavlenSpectrum(wavRange);

  DRsimInterface::hitRange timeRange = findTimeRange(hitTime);
  hit->CountTimeStruct(timeRange);

  return true;
}

void DRsimSiPMSD::EndOfEvent(G4HCofThisEvent*) {
  if ( verboseLevel>1 ) {
    G4int nofHits = fHitCollection->entries();
    G4cout
    << G4endl
    << "-------->Hits Collection: in this event they are " << nofHits
    << " hits in the tracker chambers: " << G4endl;
    for ( G4int i=0; i<nofHits; i++ ) (*fHitCollection)[i]->Print();
  }
}

DRsimInterface::hitRange DRsimSiPMSD::findWavRange(G4double en) {
  int i = 0;
  for ( ; i < fWavBin+1; i++) {
    if ( en < wavToE( (fWavlenStart - (float)i*fWavlenStep)*nm ) ) break;
  }

  if (i==0) return std::make_pair(fWavlenStart,99999.);
  else if (i==fWavBin+1) return std::make_pair(0.,fWavlenEnd);

  return std::make_pair( fWavlenStart-(float)i*fWavlenStep, fWavlenStart-(float)(i-1)*fWavlenStep );
}

DRsimInterface::hitRange DRsimSiPMSD::findTimeRange(G4double stepTime) {
  int i = 0;
  for ( ; i < fTimeBin+1; i++) {
    if ( stepTime < ( (fTimeStart + (float)i*fTimeStep)*ns ) ) break;
  }

  if (i==0) return std::make_pair(0.,fTimeStart);
  else if (i==fTimeBin+1) return std::make_pair(fTimeEnd,99999.);

  return std::make_pair( fTimeStart+(float)(i-1)*fTimeStep, fTimeStart+(float)i*fTimeStep );
}

DRsimInterface::hitXY DRsimSiPMSD::findSiPMXY(G4int SiPMnum, DRsimInterface::hitXY towerXY) {
  int x = SiPMnum/towerXY.second;
  int y = SiPMnum%towerXY.second;

  return std::make_pair(x,y);
}
