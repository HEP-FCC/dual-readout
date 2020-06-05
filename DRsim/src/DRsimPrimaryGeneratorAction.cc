#include "DRsimPrimaryGeneratorAction.hh"
#include "DRsimRunAction.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "G4AutoLock.hh"
#include "Randomize.hh"
#include <cmath>

namespace { G4Mutex DRsimPrimaryGeneratorMutex = G4MUTEX_INITIALIZER; }
int DRsimPrimaryGeneratorAction::sNumEvt = 0;
G4ThreadLocal int DRsimPrimaryGeneratorAction::sIdxEvt = 0;

using namespace std;
DRsimPrimaryGeneratorAction::DRsimPrimaryGeneratorAction(G4int seed, G4bool useHepMC, G4bool useCalib, G4bool useGPS)
: G4VUserPrimaryGeneratorAction()
{
  fSeed = seed;
  fUseHepMC = useHepMC;
  fUseCalib = useCalib;
  fUseGPS = useGPS;

  if (!fUseHepMC) {
    if (fUseGPS) initGPS();
    else initPtcGun();
  }
}

void DRsimPrimaryGeneratorAction::initPtcGun() {
  fTheta = -0.01111;
  fPhi = 0.;
  fRandX = 10.*mm;
  fRandY = 10.*mm;
  fY_0 = 0.;
  fZ_0 = 0.;
  fParticleGun = new G4ParticleGun(1);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  fElectron = particleTable->FindParticle(particleName="e-");
  fPositron = particleTable->FindParticle(particleName="e+");
  fMuon = particleTable->FindParticle(particleName="mu+");
  fPion = particleTable->FindParticle(particleName="pi+");
  fKaon0L = particleTable->FindParticle(particleName="kaon0L");
  fProton = particleTable->FindParticle(particleName="proton");
  fOptGamma = particleTable->FindParticle(particleName="opticalphoton");

  // define commands for this class
  DefineCommands();
}

void DRsimPrimaryGeneratorAction::initGPS() {
  fGPS = new G4GeneralParticleSource();
}

DRsimPrimaryGeneratorAction::~DRsimPrimaryGeneratorAction() {
  if (!fUseHepMC) {
    if (fUseGPS) delete fGPS;
    else {
      if (fParticleGun) delete fParticleGun;
      if (fMessenger) delete fMessenger;
    }
  }
}

void DRsimPrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {

  if (fUseCalib) {
    G4double x = 0;
    G4double y = (G4UniformRand()-0.5)*fRandX;
    G4double z = (G4UniformRand()-0.5)*fRandY;

    auto GunPosition = new G4ThreeVector(x, y, z);
    GunPosition->rotateY(fTheta);
    GunPosition->rotateZ(fPhi);

    G4double xTowerFront = 0;
    G4double yTowerFront = 0;
    G4double zTowerFront = 0;

    if (-fTheta < 0.98) {
      xTowerFront = 180;
      zTowerFront = 180*tan(-fTheta-1.5*M_PI/180);
    } else {
      G4double refLen = 180/cos(0.95077);
      xTowerFront = refLen*cos(-fTheta-1.5*M_PI/180);
      zTowerFront = refLen*sin(-fTheta-1.5*M_PI/180);
    }

    G4double xRelLen = xTowerFront;
    G4double yRelLen = yTowerFront-fY_0/10;
    G4double zRelLen = zTowerFront-fZ_0/10;

    double NormLen = sqrt(xRelLen*xRelLen+yRelLen*yRelLen+zRelLen*zRelLen);
    double LenRatio = 1-180/NormLen;

    fOrg.set(GunPosition->getX()+10*(LenRatio*xRelLen), GunPosition->getY()+fY_0+10*(LenRatio*yRelLen),GunPosition->getZ()+fZ_0+10*(LenRatio*zRelLen));
    fParticleGun->SetParticlePosition(fOrg); // http://www.apc.univ-paris7.fr/~franco/g4doxy/html/classG4VPrimaryGenerator.html

    fDirection.setREtaPhi(1.,0.,0.);
    fDirection.rotateY(fTheta);
    fDirection.rotateZ(fPhi);

    fParticleGun->SetParticleMomentumDirection(fDirection);

    G4AutoLock lock(&DRsimPrimaryGeneratorMutex);
    fParticleGun->GeneratePrimaryVertex(event);
    sIdxEvt = sNumEvt;
    sNumEvt++;

    return;
  }

  if (fUseGPS) {
    G4AutoLock lock(&DRsimPrimaryGeneratorMutex);
    fGPS->GeneratePrimaryVertex(event);
    sIdxEvt = sNumEvt;
    sNumEvt++;

    return;
  }

  if (fUseHepMC) {
    G4AutoLock lock(&DRsimPrimaryGeneratorMutex);
    DRsimRunAction::sHepMCreader->GeneratePrimaryVertex(event);
    sIdxEvt = sNumEvt;
    sNumEvt++;

    return;
  }

  G4double y = (G4UniformRand()-0.5)*fRandX + fY_0;//- 3.142*cm;//
  G4double z = (G4UniformRand()-0.5)*fRandY + fZ_0;//- 4.7135*cm;//10x10 mm^2
  fOrg.set(0,y,z);

  fParticleGun->SetParticlePosition(fOrg); // http://www.apc.univ-paris7.fr/~franco/g4doxy/html/classG4VPrimaryGenerator.html

  fDirection.setREtaPhi(1.,0.,0.);
  fDirection.rotateY(fTheta);
  fDirection.rotateZ(fPhi);

  fParticleGun->SetParticleMomentumDirection(fDirection);

  G4AutoLock lock(&DRsimPrimaryGeneratorMutex);
  fParticleGun->GeneratePrimaryVertex(event);
  sIdxEvt = sNumEvt;
  sNumEvt++;
}

void DRsimPrimaryGeneratorAction::DefineCommands() {
  // Define /DRsim/generator command directory using generic messenger class
  fMessenger = new G4GenericMessenger(this, "/DRsim/generator/", "Primary generator control");

  G4GenericMessenger::Command& etaCmd = fMessenger->DeclareMethodWithUnit("theta","rad",&DRsimPrimaryGeneratorAction::SetTheta,"theta of beam");
  etaCmd.SetParameterName("theta",true);
  etaCmd.SetDefaultValue("-0.01111");

  G4GenericMessenger::Command& phiCmd = fMessenger->DeclareMethodWithUnit("phi","rad",&DRsimPrimaryGeneratorAction::SetPhi,"phi of beam");
  phiCmd.SetParameterName("phi",true);
  phiCmd.SetDefaultValue("0.");

  G4GenericMessenger::Command& y0Cmd = fMessenger->DeclareMethodWithUnit("y0","cm",&DRsimPrimaryGeneratorAction::SetY0,"y_0 of beam");
  y0Cmd.SetParameterName("y0",true);
  y0Cmd.SetDefaultValue("0.");

  G4GenericMessenger::Command& z0Cmd = fMessenger->DeclareMethodWithUnit("z0","cm",&DRsimPrimaryGeneratorAction::SetZ0,"z_0 of beam");
  z0Cmd.SetParameterName("z0",true);
  z0Cmd.SetDefaultValue("0.");

  G4GenericMessenger::Command& randxCmd = fMessenger->DeclareMethodWithUnit("randx","mm",&DRsimPrimaryGeneratorAction::SetRandX,"x width of beam");
  randxCmd.SetParameterName("randx",true);
  randxCmd.SetDefaultValue("10.");

  G4GenericMessenger::Command& randyCmd = fMessenger->DeclareMethodWithUnit("randy","mm",&DRsimPrimaryGeneratorAction::SetRandY,"y width of beam");
  randyCmd.SetParameterName("randy",true);
  randyCmd.SetDefaultValue("10.");
}
