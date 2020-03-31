#include "DRsimStackingAction.hh"
#include "DRsimDetectorConstruction.hh"

#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4VProcess.hh"
#include "G4SystemOfUnits.hh"

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

  G4TouchableHandle theTouchable = track->GetTouchableHandle();
  auto fiberPos = theTouchable->GetHistory()->GetTopTransform().Inverse().TransformPoint(G4ThreeVector(0.,0.,0.));
  auto fiberAxis = theTouchable->GetHistory()->GetTopTransform().Inverse().TransformAxis(G4ThreeVector(0.,0.,1.));
  auto fiberEnd = fiberPos + fiberAxis*DRsimDetectorConstruction::sTowerH/2.;

  auto delta = step->GetDeltaPosition();
  double transportUnit = delta.dot(fiberAxis);

  if ( transportUnit < 0. ) {
    track->SetTrackStatus(G4TrackStatus::fStopAndKill);

    return;
  }

  auto toEnd = fiberEnd - track->GetPosition();
  double toEndAxis = toEnd.dot(fiberAxis);
  double maxTransport = std::floor(toEndAxis/transportUnit);
  double nTransport = maxTransport - fSafety;

  if ( nTransport < 0. ) {
    return;
  } else {
    G4Track* newTrack = new G4Track(*track);
    newTrack->CopyTrackInfo(*track);

    double velocity = track->CalculateVelocityForOpticalPhoton();
    double timeUnit = delta.mag()/velocity;

    newTrack->SetPosition( track->GetPosition() + transportUnit*nTransport*fiberAxis );
    newTrack->SetGlobalTime( track->GetGlobalTime() + timeUnit*nTransport );
    newTrack->SetLocalTime( track->GetLocalTime() + timeUnit*nTransport );
    newTrack->SetKineticEnergy( track->GetKineticEnergy() );
    newTrack->SetMomentumDirection( track->GetMomentumDirection() );
    newTrack->SetStepLength( track->GetStepLength() );
    newTrack->SetVertexPosition( track->GetVertexPosition() + transportUnit*nTransport*fiberAxis );
    newTrack->SetVertexMomentumDirection( track->GetVertexMomentumDirection() );
    newTrack->SetCreatorProcess( track->GetCreatorProcess() );

    stackManager->PushOneTrack(newTrack);

    track->SetTrackStatus(G4TrackStatus::fStopAndKill);

    return;
  }

  return;
}

void DRsimStackingAction::DefineCommands() {
  fMessenger = new G4GenericMessenger(this, "/DRsim/stack/", "stacking action control");
  G4GenericMessenger::Command& safetyCmd = fMessenger->DeclareProperty("safety",fSafety,"min number of total internal reflection");
  safetyCmd.SetParameterName("safety",true);
  safetyCmd.SetDefaultValue("2.");
}
