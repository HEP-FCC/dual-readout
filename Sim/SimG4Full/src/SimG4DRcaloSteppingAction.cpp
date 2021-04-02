#include "SimG4DRcaloSteppingAction.h"

#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4VProcess.hh"

#include "G4SystemOfUnits.hh"
#include "DD4hep/DD4hepUnits.h"

#include <stdexcept>

SimG4DRcaloSteppingAction::SimG4DRcaloSteppingAction()
: G4UserSteppingAction(), fPrevTower(0), fPrevFiber(0)
{
  m_geoSvc = GeoSvc::GetInstance();
  m_readoutName = "DRcaloSiPMreadout";

  initialize();

  if (m_geoSvc==0) throw std::runtime_error("Attempt to save Edeps while GeoSvc is not initialized!");
}

SimG4DRcaloSteppingAction::~SimG4DRcaloSteppingAction() {}

void SimG4DRcaloSteppingAction::initialize() {
  auto lcdd = m_geoSvc->lcdd();
  auto allReadouts = lcdd->readouts();
  if (allReadouts.find(m_readoutName) == allReadouts.end()) {
    throw std::runtime_error("Readout " + m_readoutName + " not found! Please check tool configuration.");
  } else {
    std::cout << "Edeps will be saved to EDM from the collection " << m_readoutName << std::endl;
  }

  fSeg = dynamic_cast<dd4hep::DDSegmentation::GridDRcalo*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());

  return;
}

void SimG4DRcaloSteppingAction::initializeEDM() {
  *mLeakages = pStore->create<edm4hep::MCParticleCollection>("Leakages");
  pWriter->registerForWrite("Leakages");

  *mEdeps = pStore->create<edm4hep::SimCalorimeterHitCollection>("SimCalorimeterEdeps");
  pWriter->registerForWrite("SimCalorimeterEdeps");

  return;
}

void SimG4DRcaloSteppingAction::UserSteppingAction(const G4Step* step) {
  G4Track* track = step->GetTrack();
  G4ParticleDefinition* particle = track->GetDefinition();

  if ( particle == G4OpticalPhoton::OpticalPhotonDefinition() ) return;

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
  float edep = step->GetTotalEnergyDeposit();

  int towerNum32 = theTouchable->GetCopyNumber( theTouchable->GetHistoryDepth()-2 );
  auto towerNum64 = fSeg->convertFirst32to64( towerNum32 );

  accumulate(fPrevTower,towerNum64,edep);

  return;
}

void SimG4DRcaloSteppingAction::accumulate(unsigned int &prev, long long int id64, float edep) {
  // search for the element
  bool found = false;
  edm4hep::SimCalorimeterHit* thePtr = nullptr;

  if ( mEdeps->size() > prev ) { // check previous element
    auto element = mEdeps->at(prev);
    if ( checkId(element, id64) ) {
      thePtr = &element;
      found = true;
    }
  }

  if (!found) { // fall back to loop
    for (unsigned int iElement = 0; iElement < mEdeps->size(); iElement++) {
      auto element = mEdeps->at(iElement);
      if ( checkId(element, id64) ) {
        found = true;
        prev = iElement;
        thePtr = &element;

        break;
      }
    }
  }

  if (!found) { // create
    auto simEdep = mEdeps->create();
    simEdep.setCellID(id64);
    simEdep.setEnergy(0.); // added later

    auto pos = fSeg->position(id64);
    simEdep.setPosition( { static_cast<float>(pos.x()*dd4hep::centimeter/dd4hep::millimeter),
                           static_cast<float>(pos.y()*dd4hep::centimeter/dd4hep::millimeter),
                           static_cast<float>(pos.z()*dd4hep::centimeter/dd4hep::millimeter) } );
    prev = mEdeps->size();
    thePtr = &simEdep;
  }

  auto edepPrev = thePtr->getEnergy();
  thePtr->setEnergy( edepPrev + edep );
}

bool SimG4DRcaloSteppingAction::checkId(edm4hep::SimCalorimeterHit edep, long long int id64) {
  return ( edep.getCellID()==static_cast<unsigned long long>(id64) );
}

void SimG4DRcaloSteppingAction::saveLeakage(G4Track* track, G4StepPoint* presteppoint) {
  auto leakage = mLeakages->create();
  leakage.setPDG( track->GetDefinition()->GetPDGEncoding() );
  leakage.setGeneratorStatus(1); // leakages naturally belong to final states
  leakage.setCharge( track->GetDefinition()->GetPDGCharge() );
  leakage.setMomentum( { static_cast<float>(track->GetMomentum().x()*CLHEP::MeV/CLHEP::GeV),
                         static_cast<float>(track->GetMomentum().y()*CLHEP::MeV/CLHEP::GeV),
                         static_cast<float>(track->GetMomentum().z()*CLHEP::MeV/CLHEP::GeV) } );
  leakage.setVertex( { static_cast<float>(presteppoint->GetPosition().x()*CLHEP::millimeter),
                       static_cast<float>(presteppoint->GetPosition().y()*CLHEP::millimeter),
                       static_cast<float>(presteppoint->GetPosition().z()*CLHEP::millimeter) } );
}
