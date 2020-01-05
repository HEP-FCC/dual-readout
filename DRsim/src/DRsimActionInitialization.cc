#include "DRsimActionInitialization.hh"
#include "DRsimPrimaryGeneratorAction.hh"
#include "DRsimRunAction.hh"
#include "DRsimEventAction.hh"
#include "DRsimSteppingAction.hh"

using namespace std;
DRsimActionInitialization::DRsimActionInitialization(int seed, G4String hepMCpath)
: G4VUserActionInitialization()
{
  fSeed = seed;
  fHepMCpath = hepMCpath;
}

DRsimActionInitialization::~DRsimActionInitialization() {}

void DRsimActionInitialization::BuildForMaster() const {
  SetUserAction(new DRsimRunAction(fSeed,fHepMCpath));
}

void DRsimActionInitialization::Build() const {
  SetUserAction(new DRsimPrimaryGeneratorAction(fSeed,fHepMCpath));
  SetUserAction(new DRsimRunAction(fSeed,fHepMCpath));

  DRsimEventAction* eventAction = new DRsimEventAction();
  SetUserAction(eventAction);

  SetUserAction(new DRsimSteppingAction(eventAction));
}
