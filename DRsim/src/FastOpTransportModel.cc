#include "FastOpTransportModel.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4ProcessManager.hh"
#include "G4OpProcessSubType.hh"

FastOpTransportModel::FastOpTransportModel(G4String name, G4Region* envelope)
: G4VFastSimulationModel(name,envelope) {
  fOpBoundaryProc = NULL;
  fCoreMaterial = NULL;
  fProcAssigned = false;
  fFiberLength = 0.;
  fSafety = 2;
  fTrkLength = 0.;
  fNtransport = 0.;
  fTransportUnit = 0.;
  fFiberAxis = G4ThreeVector(0);
  fKill = false;
  fNtotIntRefl = 0;
  fTrackId = 0;

  DefineCommands();
}

FastOpTransportModel::~FastOpTransportModel() {}

G4bool FastOpTransportModel::IsApplicable(const G4ParticleDefinition& type) {
  return &type == G4OpticalPhoton::OpticalPhotonDefinition();
}

G4bool FastOpTransportModel::ModelTrigger(const G4FastTrack& fasttrack) {
  const G4Track* track = fasttrack.GetPrimaryTrack();

  auto matPropTable = fCoreMaterial->GetMaterialPropertiesTable();

  if ( !matPropTable ) return false;

  if ( !checkTotalInternalReflection(track) ) return false; // nothing to do if the previous status is not total internal reflection

  G4TouchableHandle theTouchable = track->GetTouchableHandle();
  auto fiberPos = theTouchable->GetHistory()->GetTopTransform().Inverse().TransformPoint(G4ThreeVector(0.,0.,0.));
  fFiberAxis = theTouchable->GetHistory()->GetTopTransform().Inverse().TransformAxis(G4ThreeVector(0.,0.,1.));
  fTrkLength = track->GetTrackLength();

  if ( fTrkLength==0. ) { // kill stopped particle
    fKill = true;

    return true;
  }

  auto delta = track->GetMomentumDirection() * fTrkLength;
  fTransportUnit = delta.dot(fFiberAxis);

  if ( fTransportUnit < 0. ) { // kill backward propagation
    fKill = true;

    return true;
  }

  auto fiberEnd = fiberPos + fFiberAxis*fFiberLength/2.;
  auto toEnd = fiberEnd - track->GetPosition();
  double toEndAxis = toEnd.dot(fFiberAxis);
  double maxTransport = std::floor(toEndAxis/fTransportUnit);
  fNtransport = maxTransport - fSafety;

  if ( fNtransport < 0. ) { // require at least n = fSafety of total internal reflections at the end
    reset();

    return false;
  }

  if ( matPropTable->GetProperty(kABSLENGTH) ) {
    double attLength = matPropTable->GetProperty(kABSLENGTH)->Value( track->GetDynamicParticle()->GetTotalMomentum() );
    double nInteractionLength = fTrkLength*fNtransport/attLength;
    double nInteractionLengthLeft = -std::log( G4UniformRand() );

    if ( nInteractionLength > nInteractionLengthLeft ) { // OpAbsorption
      fKill = true;

      return true;
    }
  }

  fKill = false;

  return true;
}

void FastOpTransportModel::DoIt(const G4FastTrack& fasttrack, G4FastStep& faststep) {
  auto track = fasttrack.GetPrimaryTrack();

  if (fKill) {
    faststep.KillPrimaryTrack();
    reset();

    return;
  }

  double velocity = track->CalculateVelocityForOpticalPhoton();
  double timeUnit = fTrkLength/velocity;
  auto posShift = fTransportUnit*fNtransport*fFiberAxis;
  double timeShift = timeUnit*fNtransport;

  faststep.ProposePrimaryTrackFinalPosition( track->GetPosition() + posShift, false );
  faststep.ProposePrimaryTrackFinalTime( track->GetGlobalTime() + timeShift );
  faststep.ProposePrimaryTrackFinalKineticEnergy( track->GetKineticEnergy() );
  faststep.ProposePrimaryTrackFinalMomentumDirection( track->GetMomentumDirection(), false );
  faststep.ProposePrimaryTrackFinalPolarization( track->GetPolarization(), false );

  reset();

  return;
}

bool FastOpTransportModel::checkTotalInternalReflection(const G4Track* track) {
  if (!fProcAssigned) { // locate OpBoundaryProcess only once
    setOpBoundaryProc(track);
  }

  if ( track->GetTrackStatus()==fStopButAlive || track->GetTrackStatus()==fStopAndKill ) return false;

  if ( fOpBoundaryProc->GetStatus()==G4OpBoundaryProcessStatus::TotalInternalReflection ) {
    if ( fTrackId != track->GetTrackID() ) { // reset everything if when encountered a different track
      reset();
    }

    fTrackId = track->GetTrackID();
    fNtotIntRefl++;

    if ( fNtotIntRefl > fSafety ) { // require at least n = fSafety of total internal reflections at the beginning
      return true;
    }
  }

  return false;
}

void FastOpTransportModel::setOpBoundaryProc(const G4Track* track) {
  G4ProcessManager* pm = track->GetDefinition()->GetProcessManager();
  auto postStepProcessVector = pm->GetPostStepProcessVector();

  for (int np = 0; np < postStepProcessVector->entries(); np++) {
    auto theProcess = (*postStepProcessVector)[np];

    if ( theProcess->GetProcessType()!=fOptical || theProcess->GetProcessSubType()!=G4OpProcessSubType::fOpBoundary ) continue;

    fOpBoundaryProc = (G4OpBoundaryProcess*)theProcess;
    fProcAssigned = true;

    break;
  }

  return;
}

void FastOpTransportModel::reset() {
  fTrkLength = 0.;
  fNtransport = 0.;
  fTransportUnit = 0.;
  fFiberAxis = G4ThreeVector(0);
  fKill = false;
  fNtotIntRefl = 0;
  fTrackId = 0;
}

void FastOpTransportModel::DefineCommands() {
  fMessenger = new G4GenericMessenger(this, "/DRsim/fastOp/", "fast Op transport model control");
  G4GenericMessenger::Command& safetyCmd = fMessenger->DeclareProperty("safety",fSafety,"min number of total internal reflection");
  safetyCmd.SetParameterName("safety",true);
  safetyCmd.SetDefaultValue("2.");
}
