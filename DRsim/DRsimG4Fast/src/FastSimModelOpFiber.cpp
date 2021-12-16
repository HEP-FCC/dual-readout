#include "FastSimModelOpFiber.h"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4ProcessManager.hh"
#include "G4OpProcessSubType.hh"
#include "G4Tubs.hh"

FastSimModelOpFiber::FastSimModelOpFiber(G4String name, G4Region* envelope)
: G4VFastSimulationModel(name,envelope) {
  pOpBoundaryProc = NULL;
  fCoreMaterial = NULL;
  fProcAssigned = false;
  fSafety = 2;
  fTrkLength = 0.;
  fNtransport = 0.;
  mTransportUnit = 0.;
  mFiberAxis = G4ThreeVector(0);
  mPrevDir = G4ThreeVector(0);
  mCurrDir = G4ThreeVector(0);
  fKill = false;
  fNtotIntRefl = 0;
  mTrackId = 0;

  DefineCommands();
}

FastSimModelOpFiber::~FastSimModelOpFiber() {}

G4bool FastSimModelOpFiber::IsApplicable(const G4ParticleDefinition& type) {
  return &type == G4OpticalPhoton::OpticalPhotonDefinition();
}

G4bool FastSimModelOpFiber::ModelTrigger(const G4FastTrack& fasttrack) {
  const G4Track* track = fasttrack.GetPrimaryTrack();

  if ( mTrackId != track->GetTrackID() ) // reset when moving to the next track
    reset();

  if ( !checkTotalInternalReflection(track) )
    return false; // nothing to do if the previous status is not total internal reflection

  getCoreMaterial(track);
  auto matPropTable = fCoreMaterial->GetMaterialPropertiesTable();

  if ( !matPropTable )
    return false;

  G4TouchableHandle theTouchable = track->GetTouchableHandle();
  auto fiberPos = theTouchable->GetHistory()->GetTopTransform().Inverse().TransformPoint(G4ThreeVector(0.,0.,0.));
  mFiberAxis = theTouchable->GetHistory()->GetTopTransform().Inverse().TransformAxis(G4ThreeVector(0.,0.,1.));
  fTrkLength = track->GetTrackLength();

  if ( fTrkLength==0. ) { // kill stopped particle
    fKill = true;

    return true;
  }

  // different propagation direction (e.g. mirror)
  if ( mFiberAxis.dot(mCurrDir)*mFiberAxis.dot(mPrevDir) < 0 ) {
    reset();

    return false;
  }

  auto delta = track->GetMomentumDirection() * fTrkLength;
  mTransportUnit = delta.dot(mFiberAxis);

  G4Tubs* tubs = static_cast<G4Tubs*>(theTouchable->GetSolid());
  G4double fiberLen = 2.*tubs->GetZHalfLength();
  auto fiberEnd = ( mTransportUnit > 0. ) ? fiberPos + mFiberAxis*fiberLen/2. : fiberPos - mFiberAxis*fiberLen/2.;
  auto toEnd = fiberEnd - track->GetPosition();
  double toEndAxis = toEnd.dot(mFiberAxis);
  double maxTransport = std::floor(toEndAxis/mTransportUnit);
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
    faststep.ProposeTotalEnergyDeposited(track->GetKineticEnergy());
    faststep.KillPrimaryTrack();

    return;
  }

  double velocity = CalculateVelocityForOpticalPhoton(track);
  double timeUnit = fTrkLength/velocity;
  auto posShift = mTransportUnit*fNtransport*mFiberAxis;
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
  if (!fProcAssigned) // locate OpBoundaryProcess only once
    setOpBoundaryProc(track);

  if ( track->GetTrackStatus()==fStopButAlive || track->GetTrackStatus()==fStopAndKill ) return false;

  if ( pOpBoundaryProc->GetStatus()==G4OpBoundaryProcessStatus::TotalInternalReflection ) {
    mTrackId = track->GetTrackID();
    mPrevDir = mCurrDir;
    mCurrDir = track->GetMomentumDirection();
    fNtotIntRefl++;

    if ( fNtotIntRefl > fSafety ) // require at least n = fSafety of total internal reflections at the beginning
      return true;
  }

  return false;
}

void FastSimModelOpFiber::setOpBoundaryProc(const G4Track* track) {
  G4ProcessManager* pm = track->GetDefinition()->GetProcessManager();
  auto postStepProcessVector = pm->GetPostStepProcessVector();

  for (unsigned int np = 0; np < postStepProcessVector->entries(); np++) {
    auto theProcess = (*postStepProcessVector)[np];

    if ( theProcess->GetProcessType()!=fOptical || theProcess->GetProcessSubType()!=G4OpProcessSubType::fOpBoundary ) continue;

    pOpBoundaryProc = (G4OpBoundaryProcess*)theProcess;
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
  mTransportUnit = 0.;
  mFiberAxis = G4ThreeVector(0);
  fKill = false;
  fNtotIntRefl = 0;
  mTrackId = 0;
  mPrevDir = G4ThreeVector(0);
  mCurrDir = G4ThreeVector(0);
}

void FastSimModelOpFiber::DefineCommands() {
  fMessenger = new G4GenericMessenger(this, "/DRsim/fastOp/", "fast Op transport model control");
  G4GenericMessenger::Command& safetyCmd = fMessenger->DeclareProperty("safety",fSafety,"min number of total internal reflection");
  safetyCmd.SetParameterName("safety",true);
  safetyCmd.SetDefaultValue("2.");
}
