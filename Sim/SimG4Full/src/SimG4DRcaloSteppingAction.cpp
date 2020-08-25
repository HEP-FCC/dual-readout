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
: G4UserSteppingAction()
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

  if ( theTouchable->GetHistoryDepth()<2 ) return;

  float edep = step->GetTotalEnergyDeposit();
  float edepEle = (std::abs(pdgID)==11) ? edep : 0.;
  float edepGamma = (std::abs(pdgID)==22) ? edep : 0.;
  float edepCharged = ( std::round(std::abs(pdgCharge)) != 0. ) ? edep : 0.;

  bool isFiber = false;
  long long int fiberId64 = 0;

  auto towerNum = fSeg->convertFirst32to64( theTouchable->GetCopyNumber( theTouchable->GetHistoryDepth()-2 ) );

  int iTheta = fSeg->numEta(towerNum);
  int iPhi = fSeg->numPhi(towerNum);

  if ( theTouchable->GetHistoryDepth()==2 ) isFiber = false;

  if ( theTouchable->GetHistoryDepth()==4 ) {
    auto SiPMnum = fSeg->convertLast32to64( theTouchable->GetCopyNumber() );
    fiberId64 = towerNum | SiPMnum;

    isFiber = true;
  }

  bool isFound = false;
  bool isFiberAndFound = false;
  for (auto& iEdep : fEventData->Edeps) {
    if ( ( iEdep.iTheta == iTheta ) && ( iEdep.iPhi == iPhi ) ) {
      iEdep.accumulate(edep,edepEle,edepGamma,edepCharged);
      isFound = true;

      if (isFiber) {
        for (auto& iFiber : iEdep.fibers) {
          if ( iFiber.fiberNum==fiberId64 ) {
            iFiber.accumulate(edep,edepEle,edepGamma,edepCharged);
            isFiberAndFound = true;

            break;
          }
        }

        if (!isFiberAndFound) {
          DRsimInterface::DRsimEdepFiberData theFiber = DRsimInterface::DRsimEdepFiberData(fiberId64,edep,edepEle,edepGamma,edepCharged);
          iEdep.fibers.push_back(theFiber);
        }
      }

      break;
    }
  }

  if (!isFound) {
    DRsimInterface::DRsimEdepData theEdep = DRsimInterface::DRsimEdepData(iTheta,iPhi,edep,edepEle,edepGamma,edepCharged);

    if (isFiber) {
      DRsimInterface::DRsimEdepFiberData theFiber = DRsimInterface::DRsimEdepFiberData(fiberId64,edep,edepEle,edepGamma,edepCharged);
      theEdep.fibers.push_back(theFiber);
    }

    fEventData->Edeps.push_back(theEdep);
  }

  return;
}
