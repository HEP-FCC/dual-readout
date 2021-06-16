#include "DRcaloSiPMSD.h"
#include "DRcaloSiPMHit.h"

#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

#include "G4SystemOfUnits.hh"
#include "DD4hep/DD4hepUnits.h"

drc::DRcaloSiPMSD::DRcaloSiPMSD(const std::string aName, const std::string aReadoutName, const dd4hep::Segmentation& aSeg)
: G4VSensitiveDetector(aName), fHitCollection(0), fHCID(-1),
fWavBin(120), fTimeBin(600), fWavlenStart(900.), fWavlenEnd(300.), fTimeStart(10.), fTimeEnd(70.)
{
  collectionName.insert(aReadoutName);
  fSeg = dynamic_cast<dd4hep::DDSegmentation::GridDRcalo*>( aSeg.segmentation() );
  fWavlenStep = (fWavlenStart-fWavlenEnd)/(float)fWavBin;
  fTimeStep = (fTimeEnd-fTimeStart)/(float)fTimeBin;
}

drc::DRcaloSiPMSD::~DRcaloSiPMSD() {}

void drc::DRcaloSiPMSD::Initialize(G4HCofThisEvent* hce) {
  fHitCollection = new drc::DRcaloSiPMHitsCollection(SensitiveDetectorName,collectionName[0]);
  if (fHCID<0) { fHCID = GetCollectionID(0); }
  hce->AddHitsCollection(fHCID,fHitCollection);
}

G4bool drc::DRcaloSiPMSD::ProcessHits(G4Step* step, G4TouchableHistory*) {
  if(step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false;

  auto theTouchable = step->GetPostStepPoint()->GetTouchable();

  dd4hep::sim::Geant4VolumeManager volMgr = dd4hep::sim::Geant4Mapping::instance().volumeManager();
  dd4hep::VolumeID volID = volMgr.volumeID(theTouchable);

  G4ThreeVector global = step->GetPostStepPoint()->GetPosition();
  G4ThreeVector local = theTouchable->GetHistory()->GetTopTransform().TransformPoint( global );
  dd4hep::Position loc(local.x() * dd4hep::millimeter/CLHEP::millimeter, local.y() * dd4hep::millimeter/CLHEP::millimeter, local.z() * dd4hep::millimeter/CLHEP::millimeter);
  dd4hep::Position glob(global.x() * dd4hep::millimeter/CLHEP::millimeter, global.y() * dd4hep::millimeter/CLHEP::millimeter, global.z() * dd4hep::millimeter/CLHEP::millimeter);

  auto cID = fSeg->cellID(loc, glob, volID);

  G4int nofHits = fHitCollection->entries();
  G4double hitTime = step->GetPostStepPoint()->GetGlobalTime();
  G4double energy = step->GetTrack()->GetTotalEnergy();

  drc::DRcaloSiPMHit* hit = NULL;

  for (G4int i = 0; i < nofHits; i++) {
    if ( (*fHitCollection)[i]->GetSiPMnum()==cID ) {
      hit = (*fHitCollection)[i];
      break;
    }
  }

  if (hit==NULL) {
    hit = new DRcaloSiPMHit(fWavlenStep,fTimeStep);
    hit->SetSiPMnum(cID);

    fHitCollection->insert(hit);
  }

  hit->photonCount();

  float wavCenter = findWavCenter(energy);
  hit->CountWavlenSpectrum(wavCenter);

  float timeCenter = findTimeCenter(hitTime);
  hit->CountTimeStruct(timeCenter);

  return true;
}

float drc::DRcaloSiPMSD::findWavCenter(G4double en) {
  int i = 0;
  for ( ; i < fWavBin+1; i++) {
    if ( en < wavToE( (fWavlenStart - static_cast<float>(i)*fWavlenStep)*nm ) ) break;
  }

  if (i==0) return (fWavlenStart + 0.5*fWavlenStep);
  else if (i==fWavBin+1) return (fWavlenEnd - 0.5*fWavlenStep);

  return ( fWavlenStart-static_cast<float>(i)*fWavlenStep + fWavlenStart-static_cast<float>(i-1)*fWavlenStep )/2.;
}

float drc::DRcaloSiPMSD::findTimeCenter(G4double stepTime) {
  int i = 0;
  for ( ; i < fTimeBin+1; i++) {
    if ( stepTime < ( (fTimeStart + static_cast<float>(i)*fTimeStep)*CLHEP::ns ) ) break;
  }

  if (i==0) return (fTimeStart - 0.5*fTimeStep);
  else if (i==fTimeBin+1) return (fTimeEnd + 0.5*fTimeStep);

  return ( fTimeStart+static_cast<float>(i-1)*fTimeStep + fTimeStart+static_cast<float>(i)*fTimeStep )/2.;
}
