#include "DRsimRunAction.hh"
#include "DRsimEventAction.hh"
#include "G4AutoLock.hh"
#include "G4Threading.hh"

#include <vector>

using namespace std;

namespace { G4Mutex DRsimRunActionMutex = G4MUTEX_INITIALIZER; }
HepMCG4Reader* DRsimRunAction::sHepMCreader = 0;
DRsimRootInterface* DRsimRunAction::sRootIO = 0;
int DRsimRunAction::sNumEvt = 0;

DRsimRunAction::DRsimRunAction(int seed, G4String hepMCpath)
: G4UserRunAction()
{
  fSeed = seed;
  fHepMCpath = hepMCpath;

  G4AutoLock lock(&DRsimRunActionMutex);

  if (!sRootIO) {
    sRootIO = new DRsimRootInterface(fHepMCpath+"_"+std::to_string(fSeed)+".root");
    sRootIO->create();
  }

  if (!fHepMCpath.empty() && !sHepMCreader) {
    sHepMCreader = new HepMCG4Reader(fSeed,fHepMCpath);
  }
}

DRsimRunAction::~DRsimRunAction() {
  if (IsMaster()) {
    G4AutoLock lock(&DRsimRunActionMutex);

    if (!fHepMCpath.empty() && sHepMCreader) {
      delete sHepMCreader;
      sHepMCreader = 0;
    }

    if (sRootIO) {
      sRootIO->write();
      sRootIO->close();
      delete sRootIO;
      sRootIO = 0;
    }
  }
}

void DRsimRunAction::BeginOfRunAction(const G4Run*) {

}

void DRsimRunAction::EndOfRunAction(const G4Run*) {

}
