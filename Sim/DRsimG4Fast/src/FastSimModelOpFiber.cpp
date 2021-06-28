#include "FastSimModelOpFiber.h"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4ProcessManager.hh"
#include "G4OpProcessSubType.hh"
#include "G4Tubs.hh"

FastSimModelOpFiber::FastSimModelOpFiber(G4String name, G4Region* envelope)
: G4VFastSimulationModel(name,envelope) {
  fOpBoundaryProc = NULL;
  fCoreMaterial = NULL;
  fProcAssigned = false;
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

FastSimModelOpFiber::~FastSimModelOpFiber() {}

G4bool FastSimModelOpFiber::IsApplicable(const G4ParticleDefinition& type) {
  return &type == G4OpticalPhoton::OpticalPhotonDefinition();
}

G4bool FastSimModelOpFiber::ModelTrigger(const G4FastTrack& fasttrack) {
  const G4Track* track = fasttrack.GetPrimaryTrack();

  getCoreMaterial(track);

  auto matPropTable = fCoreMaterial->GetMaterialPropertiesTable();

  if ( !matPropTable ) return false;

  if ( !checkTotalInternalReflection(track) ) return false; // nothing to do if the previous status is not total internal reflection

  G4TouchableHandle theTouchable = track->GetTouchableHandle();
  auto fiberPos = theTouchable->GetHistory()->GetTopTransform().Inverse().TransformPoint(G4ThreeVector(0.,0.,0.));
  fFiberAxis = theTouchable->GetHistory()->GetTopTransform().Inverse().TransformAxis(G4ThreeVector(0.,0.,1.));
  fTrkLength = track->GetTrackLength();
  G4Tubs* tubs = static_cast<G4Tubs*>(theTouchable->GetSolid());
  G4double fiberLen = 2.*tubs->GetZHalfLength();

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

  auto fiberEnd = fiberPos + fFiberAxis*fiberLen/2.;
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

void FastSimModelOpFiber::DoIt(const G4FastTrack& fasttrack, G4FastStep& faststep) {
  auto track = fasttrack.GetPrimaryTrack();

  if (fKill) {
    faststep.KillPrimaryTrack();
    reset();

    return;
  }

  double velocity = CalculateVelocityForOpticalPhoton(track);
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

bool FastSimModelOpFiber::checkTotalInternalReflection(const G4Track* track) {
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

void FastSimModelOpFiber::setOpBoundaryProc(const G4Track* track) {
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

G4double FastSimModelOpFiber::CalculateVelocityForOpticalPhoton(const G4Track* track) {
  G4double velocity = CLHEP::c_light;
  G4bool update_groupvel = false;
  G4MaterialPropertyVector* groupvel = nullptr;

  // check if previous step is in the same volume
  // and get new GROUPVELOCITY table if necessary
  if ( (fCoreMaterial!=nullptr) && (groupvel==nullptr) ) {
    if ( fCoreMaterial->GetMaterialPropertiesTable() != nullptr ) {
      groupvel = fCoreMaterial->GetMaterialPropertiesTable()->GetProperty("GROUPVEL");
    }
    update_groupvel = true;
  }

  if ( groupvel != nullptr ) {
    // light velocity = c/(rindex+d(rindex)/d(log(E_phot)))
    // values stored in GROUPVEL material properties vector
    G4double current_momentum = track->GetDynamicParticle()->GetTotalMomentum();
    if ( update_groupvel ) {
      velocity = groupvel->Value(current_momentum);
    }
  }

  return velocity;
}

void FastSimModelOpFiber::getCoreMaterial(const G4Track* track) {
  auto physVol = track->GetVolume();
  auto logVol = physVol->GetLogicalVolume();

  if ( logVol->GetNoDaughters()==0 ) {
    fCoreMaterial = logVol->GetMaterial();
  } else {
    auto corePhys = logVol->GetDaughter(0);
    fCoreMaterial = corePhys->GetLogicalVolume()->GetMaterial();
  }
}

void FastSimModelOpFiber::reset() {
  fTrkLength = 0.;
  fNtransport = 0.;
  fTransportUnit = 0.;
  fFiberAxis = G4ThreeVector(0);
  fKill = false;
  fNtotIntRefl = 0;
  fTrackId = 0;
}

void FastSimModelOpFiber::DefineCommands() {
  fMessenger = new G4GenericMessenger(this, "/DRsim/fastOp/", "fast Op transport model control");
  G4GenericMessenger::Command& safetyCmd = fMessenger->DeclareProperty("safety",fSafety,"min number of total internal reflection");
  safetyCmd.SetParameterName("safety",true);
  safetyCmd.SetDefaultValue("2.");
}
