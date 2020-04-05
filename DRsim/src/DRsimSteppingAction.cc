#include "DRsimSteppingAction.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4VProcess.hh"
#include "G4OpProcessSubType.hh"
#include "G4OpBoundaryProcess.hh"

DRsimSteppingAction::DRsimSteppingAction(DRsimEventAction* eventAction, DRsimStackingAction* stackAct)
: G4UserSteppingAction(), fEventAction(eventAction), fStackAction(stackAct)
{}

DRsimSteppingAction::~DRsimSteppingAction() {}

void DRsimSteppingAction::UserSteppingAction(const G4Step* step) {
  G4Track* track = step->GetTrack();
  G4ParticleDefinition* particle = track->GetDefinition();

  if ( particle == G4OpticalPhoton::OpticalPhotonDefinition() ) {
    if ( fpSteppingManager->GetphysIntLength()==0. ) return; // not in optical material

    auto currentProc = fpSteppingManager->GetfCurrentProcess();
    if ( currentProc->GetProcessType()!=fOptical || currentProc->GetProcessSubType()!=G4OpProcessSubType::fOpBoundary ) return; // not OpBoundary process

    G4OpBoundaryProcess* opProc = (G4OpBoundaryProcess*)currentProc;

    if ( opProc->GetStatus()!=G4OpBoundaryProcessStatus::TotalInternalReflection ) return; // not total internal reflection

    if ( !IsFiberMaterial( track->GetMaterial()->GetName() ) ) return;
    if ( !IsFiberMaterial( track->GetNextMaterial()->GetName() ) ) return; // not in fibre

    if ( track->GetTouchableHandle()->GetHistoryDepth()!=3 ) return; // not in core

    fStackAction->transportOp(step);
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

    G4VPhysicalVolume* motherTower = GetMotherTower(theTouchable);
    fEdep.iTheta = GetTowerITheta(motherTower->GetName());
    fEdep.iPhi = GetTowerIPhi(motherTower);

    G4double pdgCharge = particle->GetPDGCharge();

    fEdep.Edep = step->GetTotalEnergyDeposit();
    fEdep.EdepEle = (std::abs(pdgID)==11) ? fEdep.Edep : 0.;
    fEdep.EdepGamma = (std::abs(pdgID)==22) ? fEdep.Edep : 0.;
    fEdep.EdepCharged = ( std::round(std::abs(pdgCharge)) != 0. ) ? fEdep.Edep : 0.;

    if ( fEdep.Edep > 0. ) fEventAction->fillEdeps(fEdep);
  }

  return;
}

bool DRsimSteppingAction::IsFiberMaterial(G4String matName) {
  return ( matName=="Polystyrene" || matName=="FluorinatedPolymer" || matName=="PMMA" );
}
