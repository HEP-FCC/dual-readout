#include "DRsimSteppingAction.hh"
#include "DRsimDetectorConstruction.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4VProcess.hh"
#include "G4OpProcessSubType.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4EmProcessSubType.hh"

DRsimSteppingAction::DRsimSteppingAction(DRsimEventAction* eventAction)
: G4UserSteppingAction(), fEventAction(eventAction)
{}

DRsimSteppingAction::~DRsimSteppingAction() {}

void DRsimSteppingAction::UserSteppingAction(const G4Step* step) {
  G4Track* track = step->GetTrack();
  G4ParticleDefinition* particle = track->GetDefinition();

  if ( particle == G4OpticalPhoton::OpticalPhotonDefinition() ) {
    if ( track->GetMaterial()->GetName()=="FluorinatedPolymer" || track->GetNextMaterial()->GetName()=="FluorinatedPolymer" ) {
      auto creatorProcess = track->GetCreatorProcess();
      if ( creatorProcess->GetProcessType()==fElectromagnetic && creatorProcess->GetProcessSubType()==G4EmProcessSubType::fScintillation ) {
        track->SetTrackStatus(G4TrackStatus::fStopAndKill); // AdHoc treatment for Xtalk
        return;
      }
    }
  } else {
    G4int pdgID = particle->GetPDGEncoding();
    G4StepPoint* presteppoint = step->GetPreStepPoint();
    G4StepPoint* poststeppoint = step->GetPostStepPoint();
    G4LogicalVolume* preVol = presteppoint->GetPhysicalVolume()->GetLogicalVolume();
    G4TouchableHandle theTouchable = presteppoint->GetTouchableHandle();

    if (poststeppoint->GetStepStatus() == fWorldBoundary) {
      fLeak.E = track->GetTotalEnergy();
      fLeak.px = track->GetMomentum().x();
      fLeak.py = track->GetMomentum().y();
      fLeak.pz = track->GetMomentum().z();
      fLeak.vx = presteppoint->GetPosition().x();
      fLeak.vy = presteppoint->GetPosition().y();
      fLeak.vz = presteppoint->GetPosition().z();
      fLeak.vt = presteppoint->GetGlobalTime();
      fLeak.pdgId = track->GetDefinition()->GetPDGEncoding();

      fEventAction->fillLeaks(fLeak);
    }

    G4String matName = preVol->GetMaterial()->GetName();

    if ( matName=="G4_Galactic" || matName=="Air" ) return;

    DRsimInterface::DRsimEdepData theEdep = DRsimInterface::DRsimEdepData();

    G4VPhysicalVolume* motherTower = GetMotherTower(theTouchable);
    auto towerName = motherTower->GetName();
    theEdep.iTheta = GetTowerITheta(towerName);
    theEdep.iPhi = GetTowerIPhi(motherTower);

    G4double pdgCharge = particle->GetPDGCharge();

    theEdep.Edep = step->GetTotalEnergyDeposit();
    theEdep.EdepEle = (std::abs(pdgID)==11) ? theEdep.Edep : 0.;
    theEdep.EdepGamma = (std::abs(pdgID)==22) ? theEdep.Edep : 0.;
    theEdep.EdepCharged = ( std::round(std::abs(pdgCharge)) != 0. ) ? theEdep.Edep : 0.;

    bool IsFiber = false;
    DRsimInterface::DRsimEdepFiberData theFiber = DRsimInterface::DRsimEdepFiberData();

    if ( (matName=="Polystyrene" || matName=="PMMA") && theTouchable->GetHistoryDepth()==3 ) {
      IsFiber = true;

      auto theDetector = DRsimDetectorConstruction::GetInstance();
      DRsimInterface::DRsimTowerProperty theTower;
      G4int towerITheta = std::stoi(towerName.substr(3));

      if (fIsR && fIsB) theTower = theDetector->GetTowerPropBR().at(towerITheta);
      else if (!fIsR && fIsB) theTower = theDetector->GetTowerPropBL().at(towerITheta);
      else if (fIsR && !fIsB) theTower = theDetector->GetTowerPropER().at(towerITheta);
      else if (!fIsR && !fIsB) theTower = theDetector->GetTowerPropEL().at(towerITheta);

      theFiber.fiberNum = theTouchable->GetCopyNumber();
      auto fiberXY = DRsimDetectorConstruction::findSiPMXY(theFiber.fiberNum, theTower.towerXY);
      theFiber.x = fiberXY.first;
      theFiber.y = fiberXY.second;
      theFiber.IsCerenkov = (matName=="Polystyrene") ? false : true;

      theFiber.Edep = step->GetTotalEnergyDeposit();
      theFiber.EdepEle = (std::abs(pdgID)==11) ? theFiber.Edep : 0.;
      theFiber.EdepGamma = (std::abs(pdgID)==22) ? theFiber.Edep : 0.;
      theFiber.EdepCharged = ( std::round(std::abs(pdgCharge)) != 0. ) ? theFiber.Edep : 0.;
    }

    if ( theEdep.Edep > 0. ) fEventAction->fillEdeps(theEdep,theFiber,IsFiber);
  }

  return;
}
