#include "SimG4DRcaloSteppingAction.h"

#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4VProcess.hh"
#include "G4OpProcessSubType.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4EmProcessSubType.hh"

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

void SimG4DRcaloSteppingAction::UserSteppingAction(const G4Step* step) {
  G4Track* track = step->GetTrack();
  G4ParticleDefinition* particle = track->GetDefinition();

  if ( particle == G4OpticalPhoton::OpticalPhotonDefinition() ) return;

  G4int pdgID = particle->GetPDGEncoding();
  G4double pdgCharge = particle->GetPDGCharge();
  G4StepPoint* presteppoint = step->GetPreStepPoint();
  G4StepPoint* poststeppoint = step->GetPostStepPoint();
  G4TouchableHandle theTouchable = presteppoint->GetTouchableHandle();

  // leakage particles
  if (poststeppoint->GetStepStatus() == fWorldBoundary) {
    DRsimInterface::DRsimLeakageData leak = DRsimInterface::DRsimLeakageData();

    leak.E = track->GetTotalEnergy();
    leak.px = track->GetMomentum().x();
    leak.py = track->GetMomentum().y();
    leak.pz = track->GetMomentum().z();
    leak.vx = presteppoint->GetPosition().x();
    leak.vy = presteppoint->GetPosition().y();
    leak.vz = presteppoint->GetPosition().z();
    leak.vt = presteppoint->GetGlobalTime();
    leak.pdgId = track->GetDefinition()->GetPDGEncoding();

    fEventData->leaks.push_back(leak);

    return;
  }

  if ( theTouchable->GetHistoryDepth()<2 ) return; // skip particles in the world or assembly volume

  // MC truth energy deposits
  float edep = step->GetTotalEnergyDeposit();
  float edepEle = (std::abs(pdgID)==11) ? edep : 0.;
  float edepGamma = (std::abs(pdgID)==22) ? edep : 0.;
  float edepCharged = ( std::round(std::abs(pdgCharge)) != 0. ) ? edep : 0.;

  int towerNum32 = theTouchable->GetCopyNumber( theTouchable->GetHistoryDepth()-2 );
  auto towerNum64 = fSeg->convertFirst32to64( towerNum32 );

  accumulate(fEventData->Edeps,fPrevTower,towerNum64,edep,edepEle,edepGamma,edepCharged);

  bool isFiber = false;
  long long int fiberId64 = 0;

  // check the type
  if ( theTouchable->GetHistoryDepth()==4 ) {
    auto SiPMnum = fSeg->convertLast32to64( theTouchable->GetCopyNumber() );
    fiberId64 = towerNum64 | SiPMnum;

    isFiber = true;
  }

  if (!isFiber) return; // done if the type is not fiber

  accumulate(fEventData->Edeps.at(fPrevTower).fibers,fPrevFiber,fiberId64,edep,edepEle,edepGamma,edepCharged);

  return;
}

template <typename T>
void SimG4DRcaloSteppingAction::accumulate(std::vector<T> &input, unsigned int &prev, long long int id64,
                                           float edep, float edepEle, float edepGamma, float edepCharged) {
  // search for the element
  typename std::vector<T>::iterator thePtr = input.begin();
  bool found = false;

  if ( input.size() > prev ) { // check previous element
    T element = input.at(prev);
    if ( checkId(element, id64) ) {
      std::advance(thePtr,prev);
      found = true;
    }
  }

  if (!found) { // fall back to loop
    for (unsigned int iElement = 0; iElement < input.size(); iElement++) {
      T element = input.at(iElement);
      if ( checkId(element, id64) ) {
        found = true;
        prev = iElement;
        std::advance(thePtr,prev);

        break;
      }
    }
  }

  if (!found) { // create
    T theEdep = create(id64,input);
    prev = input.size();
    input.push_back(theEdep);
    thePtr = input.begin();
    std::advance(thePtr,prev);
  }

  thePtr->accumulate(edep,edepEle,edepGamma,edepCharged);
}

bool SimG4DRcaloSteppingAction::checkId(DRsimInterface::DRsimEdepData edep, long long int id64) {
  int iTheta = fSeg->numEta(id64);
  int iPhi = fSeg->numPhi(id64);
  return ( edep.iTheta==iTheta && edep.iPhi==iPhi );
}

bool SimG4DRcaloSteppingAction::checkId(DRsimInterface::DRsimEdepFiberData edep, long long int id64) {
  return edep.fiberNum==id64;
}

DRsimInterface::DRsimEdepData SimG4DRcaloSteppingAction::create(long long int id64, std::vector<DRsimInterface::DRsimEdepData>&) {
  int iTheta = fSeg->numEta(id64);
  int iPhi = fSeg->numPhi(id64);

  return DRsimInterface::DRsimEdepData(iTheta,iPhi);
}

DRsimInterface::DRsimEdepFiberData SimG4DRcaloSteppingAction::create(long long int id64, std::vector<DRsimInterface::DRsimEdepFiberData>&) {
  return DRsimInterface::DRsimEdepFiberData(id64);
}
