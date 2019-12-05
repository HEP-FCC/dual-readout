#include "DRsimSteppingAction.hh"
#include "DRsimEventAction.hh"
#include "DRsimDetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

DRsimSteppingAction::DRsimSteppingAction(DRsimEventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fScoringVolume(0),
  fEdep(0.), fEdepEle(0.), fEdepGamma(0.), fEdepCharged(0.)
{}

DRsimSteppingAction::~DRsimSteppingAction() {}

void DRsimSteppingAction::UserSteppingAction(const G4Step* step)
{
  particle = step->GetTrack()->GetDefinition();
  if (particle == G4OpticalPhoton::OpticalPhotonDefinition()) return;

  pre_volume = step->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume();
  presteppoint = step->GetPreStepPoint();
  poststeppoint = step->GetPostStepPoint();
  theTouchable = presteppoint->GetTouchableHandle();

  if (poststeppoint->GetStepStatus() == fWorldBoundary) {
    fEventAction->AddLeakage(pdgID,step->GetPreStepPoint()->GetKineticEnergy(),step->GetPreStepPoint()->GetTotalEnergy());
  }

  G4String matName = pre_volume->GetMaterial()->GetName();

  if ( matName=="G4_Galactic" || matName=="Air" ) return;

  motherTower = GetMotherTower(matName,theTouchable);
  towerIEta = GetTowerIEta(motherTower->GetName());
  towerIPhi = GetTowerIPhi(motherTower);

  pdgID = particle->GetPDGEncoding();
  pdgCharge = particle->GetPDGCharge();
  fEdep = step->GetTotalEnergyDeposit();
  fEdepEle = (std::abs(pdgID)==11) ? fEdep : 0.;
  fEdepGamma = (std::abs(pdgID)==22) ? fEdep : 0.;
  fEdepCharged = ( std::round(std::abs(pdgCharge)) != 0. ) ? fEdep : 0.;
  sl = step->GetStepLength();

  if ( fEdep > 0. ) {
    fEventAction->AddStepInfo(fEdep,fEdepEle,fEdepGamma,fEdepCharged,towerIEta,towerIPhi);
  }

  return;
}
