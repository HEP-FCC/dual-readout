#include "DRcaloSiPMSD.h"
#include "DRcaloSiPMHit.h"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

ddDRcalo::DRcaloSiPMSD::DRcaloSiPMSD(const std::string aName, const std::string aReadoutName, const dd4hep::Segmentation& aSeg)
: G4VSensitiveDetector(aName), fHitCollection(0), fSeg(aSeg), fHCID(-1),
fWavBin(120), fTimeBin(600), fWavlenStart(900.), fWavlenEnd(300.), fTimeStart(10.), fTimeEnd(70.)
{
  collectionName.insert(aReadoutName);
  fWavlenStep = (fWavlenStart-fWavlenEnd)/(float)fWavBin;
  fTimeStep = (fTimeEnd-fTimeStart)/(float)fTimeBin;
}

ddDRcalo::DRcaloSiPMSD::~DRcaloSiPMSD() {}

void ddDRcalo::DRcaloSiPMSD::Initialize(G4HCofThisEvent* hce) {
  fHitCollection = new ddDRcalo::DRcaloSiPMHitsCollection(SensitiveDetectorName,collectionName[0]);
  if (fHCID<0) { fHCID = GetCollectionID(0); }
  hce->AddHitsCollection(fHCID,fHitCollection);
}

G4bool ddDRcalo::DRcaloSiPMSD::ProcessHits(G4Step* step, G4TouchableHistory*) {
  if(step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false;

  uint64_t SiPMnum = static_cast<uint64_t>(step->GetPostStepPoint()->GetTouchable()->GetVolume(1)->GetCopyNo());
  G4int nofHits = fHitCollection->entries();
  G4double hitTime = step->GetPostStepPoint()->GetGlobalTime();
  G4double energy = step->GetTrack()->GetTotalEnergy();

  ddDRcalo::DRcaloSiPMHit* hit = NULL;

  for (G4int i = 0; i < nofHits; i++) {
    if ( (*fHitCollection)[i]->GetSiPMnum()==SiPMnum ) {
      hit = (*fHitCollection)[i];
      break;
    }
  }

  if (hit==NULL) {
    hit = new DRcaloSiPMHit(fWavBin,fTimeBin);

    G4ThreeVector pos = step->GetPostStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform().Inverse().TransformPoint(G4ThreeVector(0.,0.,0.));

    hit->SetSiPMnum(SiPMnum);
    hit->SetSiPMpos( dd4hep::Position(pos.x(),pos.y(),pos.z()) ); // CLHEP::Hep3Vector to ROOT::Math::XYZVector

    fHitCollection->insert(hit);
  }

  hit->photonCount();

  DRsimInterface::hitRange wavRange = findWavRange(energy);
  hit->CountWavlenSpectrum(wavRange);

  DRsimInterface::hitRange timeRange = findTimeRange(hitTime);
  hit->CountTimeStruct(timeRange);

  return true;
}

DRsimInterface::hitRange ddDRcalo::DRcaloSiPMSD::findWavRange(G4double en) {
  int i = 0;
  for ( ; i < fWavBin+1; i++) {
    if ( en < wavToE( (fWavlenStart - (float)i*fWavlenStep)*nm ) ) break;
  }

  if (i==0) return std::make_pair(fWavlenStart,99999.);
  else if (i==fWavBin+1) return std::make_pair(0.,fWavlenEnd);

  return std::make_pair( fWavlenStart-(float)i*fWavlenStep, fWavlenStart-(float)(i-1)*fWavlenStep );
}

DRsimInterface::hitRange ddDRcalo::DRcaloSiPMSD::findTimeRange(G4double stepTime) {
  int i = 0;
  for ( ; i < fTimeBin+1; i++) {
    if ( stepTime < ( (fTimeStart + (float)i*fTimeStep)*ns ) ) break;
  }

  if (i==0) return std::make_pair(0.,fTimeStart);
  else if (i==fTimeBin+1) return std::make_pair(fTimeEnd,99999.);

  return std::make_pair( fTimeStart+(float)(i-1)*fTimeStep, fTimeStart+(float)i*fTimeStep );
}
