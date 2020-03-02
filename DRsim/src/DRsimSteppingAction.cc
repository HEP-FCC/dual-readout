#include "DRsimSteppingAction.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

DRsimSteppingAction::DRsimSteppingAction(DRsimEventAction* eventAction)
: G4UserSteppingAction(), fEventAction(eventAction)
{}

DRsimSteppingAction::~DRsimSteppingAction() {}

void DRsimSteppingAction::UserSteppingAction(const G4Step* step) {
  if (step->GetTrack()->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) return;

  G4Track* track = step->GetTrack();
  G4ParticleDefinition* particle = track->GetDefinition();
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

  return;
}
