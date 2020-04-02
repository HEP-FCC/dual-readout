#include "DRsimStackingAction.hh"
#include "DRsimDetectorConstruction.hh"

#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4VProcess.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"

DRsimStackingAction::DRsimStackingAction() {
  fSafety = 2.;

  DefineCommands();
}

G4ClassificationOfNewTrack DRsimStackingAction::ClassifyNewTrack(const G4Track*) {
  return fUrgent;
}

void DRsimStackingAction::transportOp(const G4Step* step) {
  G4Track* track = step->GetTrack();
  G4ParticleDefinition* particle = track->GetDefinition();

  if ( particle != G4OpticalPhoton::OpticalPhotonDefinition() ) return;

  auto matPropTable = track->GetMaterial()->GetMaterialPropertiesTable();

  if ( !matPropTable || !matPropTable->GetProperty(kABSLENGTH) ) return;

  G4TouchableHandle theTouchable = track->GetTouchableHandle();
  auto fiberPos = theTouchable->GetHistory()->GetTopTransform().Inverse().TransformPoint(G4ThreeVector(0.,0.,0.));
  auto fiberAxis = theTouchable->GetHistory()->GetTopTransform().Inverse().TransformAxis(G4ThreeVector(0.,0.,1.));

  auto delta = step->GetDeltaPosition();
  double transportUnit = delta.dot(fiberAxis);

  if ( transportUnit < 0. ) { // kill backward propagation
    track->SetTrackStatus(G4TrackStatus::fStopAndKill);

    return;
  }

  double mag = delta.mag();
  auto fiberEnd = fiberPos + fiberAxis*DRsimDetectorConstruction::sTowerH/2.;
  auto toEnd = fiberEnd - track->GetPosition();
  double toEndAxis = toEnd.dot(fiberAxis);
  double maxTransport = std::floor(toEndAxis/transportUnit);
  double nTransport = maxTransport - fSafety;

  if ( nTransport < 0. ) return; // require at least n = fSafety of total internal reflections

  double attLength = matPropTable->GetProperty(kABSLENGTH)->Value( track->GetDynamicParticle()->GetTotalMomentum() );
  double nInteractionLength = mag*nTransport/attLength;
  double nInteractionLengthLeft = -std::log( G4UniformRand() );

  if ( nInteractionLength > nInteractionLengthLeft ) {
    track->SetTrackStatus(G4TrackStatus::fStopAndKill); // OpAbsorption

    return;
  }

  G4Track* newTrack = new G4Track(*track);
  newTrack->CopyTrackInfo(*track);

  double velocity = track->CalculateVelocityForOpticalPhoton();
  double timeUnit = mag/velocity;
  auto posShift = transportUnit*nTransport*fiberAxis;
  double timeShift = timeUnit*nTransport;

  newTrack->SetPosition( track->GetPosition() + posShift );
  newTrack->SetGlobalTime( track->GetGlobalTime() + timeShift );
  newTrack->SetLocalTime( track->GetLocalTime() + timeShift );
  newTrack->SetKineticEnergy( track->GetKineticEnergy() );
  newTrack->SetMomentumDirection( track->GetMomentumDirection() );
  newTrack->SetStepLength( track->GetStepLength() );
  newTrack->SetVertexPosition( track->GetVertexPosition() + posShift );
  newTrack->SetVertexMomentumDirection( track->GetVertexMomentumDirection() );
  newTrack->SetCreatorProcess( track->GetCreatorProcess() );

  stackManager->PushOneTrack(newTrack);

  track->SetTrackStatus(G4TrackStatus::fStopAndKill);

  return;
}

void DRsimStackingAction::DefineCommands() {
  fMessenger = new G4GenericMessenger(this, "/DRsim/stack/", "stacking action control");
  G4GenericMessenger::Command& safetyCmd = fMessenger->DeclareProperty("safety",fSafety,"min number of total internal reflection");
  safetyCmd.SetParameterName("safety",true);
  safetyCmd.SetDefaultValue("2.");
}
