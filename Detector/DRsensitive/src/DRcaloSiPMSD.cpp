#include "DRcaloSiPMSD.h"
#include "DRcaloSiPMHit.h"

#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

#ifdef HAVE_GEANT4_UNITS
#define MM_2_CM 1.0
#else
#define MM_2_CM 0.1
#endif

ddDRcalo::DRcaloSiPMSD::DRcaloSiPMSD(const std::string aName, const std::string aReadoutName, const dd4hep::Segmentation& aSeg)
: G4VSensitiveDetector(aName), fHitCollection(0), fHCID(-1),
fWavBin(120), fTimeBin(600), fWavlenStart(900.), fWavlenEnd(300.), fTimeStart(10.), fTimeEnd(70.)
{
  collectionName.insert(aReadoutName);
  fSeg = dynamic_cast<dd4hep::DDSegmentation::GridDRcalo*>( aSeg.segmentation() );
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

  auto theTouchable = step->GetPostStepPoint()->GetTouchable();

  dd4hep::sim::Geant4VolumeManager volMgr = dd4hep::sim::Geant4Mapping::instance().volumeManager();
  dd4hep::VolumeID volID = volMgr.volumeID(theTouchable);

  G4ThreeVector global = step->GetPostStepPoint()->GetPosition();
  G4ThreeVector local = theTouchable->GetHistory()->GetTopTransform().TransformPoint( global );
  dd4hep::Position loc(local.x() * MM_2_CM, local.y() * MM_2_CM, local.z() * MM_2_CM);
  dd4hep::Position glob(global.x() * MM_2_CM, global.y() * MM_2_CM, global.z() * MM_2_CM);

  auto cID = fSeg->cellID(loc, glob, volID);

  G4int nofHits = fHitCollection->entries();
  G4double hitTime = step->GetPostStepPoint()->GetGlobalTime();
  G4double energy = step->GetTrack()->GetTotalEnergy();

  ddDRcalo::DRcaloSiPMHit* hit = NULL;

  for (G4int i = 0; i < nofHits; i++) {
    if ( (*fHitCollection)[i]->GetSiPMnum()==cID ) {
      hit = (*fHitCollection)[i];
      break;
    }
  }

  if (hit==NULL) {
    hit = new DRcaloSiPMHit(fWavBin,fTimeBin);
    hit->SetSiPMnum(cID);

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
    if ( stepTime < ( (fTimeStart + (float)i*fTimeStep)*CLHEP::ns ) ) break;
  }

  if (i==0) return std::make_pair(0.,fTimeStart);
  else if (i==fTimeBin+1) return std::make_pair(fTimeEnd,99999.);

  return std::make_pair( fTimeStart+(float)(i-1)*fTimeStep, fTimeStart+(float)i*fTimeStep );
}
