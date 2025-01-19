#include "SimG4DRcaloSteppingAction.h"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4VProcess.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#include "DD4hep/DD4hepUnits.h"

#include "DD4hep/Detector.h"
#include "DD4hep/OpticalSurfaces.h"
#include "DDG4/Geant4Mapping.h"

namespace drc {

SimG4DRcaloSteppingAction::SimG4DRcaloSteppingAction()
: G4UserSteppingAction(), fPrevTower(0), fPrevFiber(0), fPrevId(0) {
  // get static methods
  dd4hep::sim::Geant4GeometryInfo& info = dd4hep::sim::Geant4Mapping::instance().data();
  dd4hep::Detector& description = dd4hep::Detector::getInstance();

  dd4hep::OpticalSurfaceManager surfMgr = description.surfaceManager();
  dd4hep::OpticalSurface filterSurfProp = surfMgr.opticalSurface("/world/DRcalo#FilterSurf");

  if ( filterSurfProp.isValid() )
    fFilterSurf = info.g4OpticalSurfaces[filterSurfProp.access()];
}

SimG4DRcaloSteppingAction::~SimG4DRcaloSteppingAction() {}

void SimG4DRcaloSteppingAction::UserSteppingAction(const G4Step* step) {
  G4Track* track = step->GetTrack();
  G4ParticleDefinition* particle = track->GetDefinition();

  if ( particle == G4OpticalPhoton::OpticalPhotonDefinition() ) {
    // apply yellow filter here to save CPU efficiency
    // prevent double-counting filter efficiency
    if ( fPrevId==track->GetTrackID() )
      return;

    auto* mpt = track->GetMaterial()->GetMaterialPropertiesTable();

    // only applied to the scintillation channel
    if ( mpt && mpt->ConstPropertyExists(kSCINTILLATIONYIELD) ) {
      if ( fFilterSurf && fFilterSurf->GetMaterialPropertiesTable()->GetProperty(kTRANSMITTANCE) ) {
        double photonMomentum = track->GetDynamicParticle()->GetTotalMomentum();
        auto* transvec = fFilterSurf->GetMaterialPropertiesTable()->GetProperty(kTRANSMITTANCE);
        double transmittance = transvec->Value(photonMomentum);
        fPrevId = track->GetTrackID();

        if ( G4UniformRand() > transmittance )
          track->SetTrackStatus(G4TrackStatus::fStopAndKill);
      }
    }

    return;
  }

  G4StepPoint* presteppoint = step->GetPreStepPoint();
  G4StepPoint* poststeppoint = step->GetPostStepPoint();
  G4TouchableHandle theTouchable = presteppoint->GetTouchableHandle();

  // leakage particles
  if (poststeppoint->GetStepStatus() == fWorldBoundary) {
    saveLeakage(track,presteppoint);

    return;
  }

  if ( theTouchable->GetHistoryDepth()<2 ) return; // skip particles in the world or assembly volume

  // MC truth energy deposits
  float edep = step->GetTotalEnergyDeposit()*CLHEP::MeV/CLHEP::GeV;

  int towerNum32 = theTouchable->GetCopyNumber( theTouchable->GetHistoryDepth()-2 );
  auto towerNum64 = pSeg->convertFirst32to64( towerNum32 );

  if (edep > m_thres) {
    auto simEdep3d = m_Edeps3d->create();
    simEdep3d.setCellID( static_cast<unsigned long long>(towerNum64) );
    simEdep3d.setEnergy(edep);

    auto& pos = presteppoint->GetPosition();
    simEdep3d.setPosition( { static_cast<float>(pos.x()*CLHEP::millimeter),
                             static_cast<float>(pos.y()*CLHEP::millimeter),
                             static_cast<float>(pos.z()*CLHEP::millimeter) } );
  }

  accumulate(fPrevTower,towerNum64,edep);

  return;
}

void SimG4DRcaloSteppingAction::accumulate(unsigned int &prev, dd4hep::DDSegmentation::CellID& id64, float edep) {
  // search for the element
  bool found = false;
  edm4hep::MutableSimCalorimeterHit* thePtr = nullptr;

  if ( m_Edeps->size() > prev ) { // check previous element
    auto element = m_Edeps->at(prev);
    if ( checkId(element, id64) ) {
      thePtr = &element;
      found = true;
    }
  }

  if (!found) { // fall back to loop
    for (unsigned int iElement = 0; iElement < m_Edeps->size(); iElement++) {
      auto element = m_Edeps->at(iElement);
      if ( checkId(element, id64) ) {
        found = true;
        prev = iElement;
        thePtr = &element;

        break;
      }
    }
  }

  if (!found) { // create
    auto simEdep = m_Edeps->create();
    simEdep.setCellID( static_cast<unsigned long long>(id64) );
    simEdep.setEnergy(0.); // added later

    auto pos = pSeg->position(id64);
    simEdep.setPosition( { static_cast<float>(pos.x()*CLHEP::millimeter/dd4hep::millimeter),
                           static_cast<float>(pos.y()*CLHEP::millimeter/dd4hep::millimeter),
                           static_cast<float>(pos.z()*CLHEP::millimeter/dd4hep::millimeter) } );
    prev = m_Edeps->size();
    thePtr = &simEdep;

    auto edepPrev = thePtr->getEnergy();
    thePtr->setEnergy( edepPrev + edep );
  }

}

bool SimG4DRcaloSteppingAction::checkId(edm4hep::SimCalorimeterHit edep, dd4hep::DDSegmentation::CellID& id64) {
  return ( edep.getCellID()==static_cast<unsigned long long>(id64) );
}

void SimG4DRcaloSteppingAction::saveLeakage(G4Track* track, G4StepPoint* presteppoint) {
  auto leakage = m_Leakages->create();
  leakage.setPDG( track->GetDefinition()->GetPDGEncoding() );
  leakage.setGeneratorStatus(1); // leakages naturally belong to final states
  leakage.setCharge( track->GetDefinition()->GetPDGCharge() );
  leakage.setMass( track->GetDefinition()->GetPDGMass()*CLHEP::MeV/CLHEP::GeV );
  leakage.setMomentum( { static_cast<float>(track->GetMomentum().x()*CLHEP::MeV/CLHEP::GeV),
                         static_cast<float>(track->GetMomentum().y()*CLHEP::MeV/CLHEP::GeV),
                         static_cast<float>(track->GetMomentum().z()*CLHEP::MeV/CLHEP::GeV) } );
  leakage.setVertex( { static_cast<float>(presteppoint->GetPosition().x()*CLHEP::millimeter),
                       static_cast<float>(presteppoint->GetPosition().y()*CLHEP::millimeter),
                       static_cast<float>(presteppoint->GetPosition().z()*CLHEP::millimeter) } );
}

} // namespace drc
