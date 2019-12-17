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
  SetUserAction(new DRsimRunAction(fSeed,fWavBin,fTimeBin));
}

void DRsimActionInitialization::Build() const {
  SetUserAction(new DRsimPrimaryGeneratorAction(fSeed,fHepMCpath));
  SetUserAction(new DRsimRunAction(fSeed,fWavBin,fTimeBin));

  DRsimEventAction* eventAction = new DRsimEventAction();
  SetUserAction(eventAction);

  SetUserAction(new DRsimSteppingAction(eventAction));
}
