#include "DRsimActionInitialization.hh"
#include "DRsimPrimaryGeneratorAction.hh"
#include "DRsimRunAction.hh"
#include "DRsimEventAction.hh"
#include "DRsimSteppingAction.hh"

using namespace std;
DRsimActionInitialization::DRsimActionInitialization(int seed, G4int wavBin, G4int timeBin, G4String hepMCpath)
: G4VUserActionInitialization()
{
  fSeed = seed;
  fWavBin = wavBin;
  fTimeBin = timeBin;
  fHepMCpath = hepMCpath;
}

DRsimActionInitialization::~DRsimActionInitialization() {}

void DRsimActionInitialization::BuildForMaster() const {
  DRsimEventAction* eventAction = new DRsimEventAction(fWavBin,fTimeBin);
  SetUserAction(new DRsimRunAction(eventAction,fSeed,fWavBin,fTimeBin));
}

void DRsimActionInitialization::Build() const {
  G4cout << "Primary Generator Action Started" << G4endl;
  SetUserAction(new DRsimPrimaryGeneratorAction(fSeed,fHepMCpath));
  G4cout << "Initialize Event Action" << G4endl;
  DRsimEventAction* eventAction = new DRsimEventAction(fWavBin,fTimeBin);
  SetUserAction(eventAction);
  G4cout << "Initialize Run Action" << G4endl;

  SetUserAction(new DRsimRunAction(eventAction,fSeed,fWavBin,fTimeBin));

  SetUserAction(new DRsimSteppingAction(eventAction));
}
