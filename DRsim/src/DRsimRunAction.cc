#include "DRsimRunAction.hh"
#include "DRsimEventAction.hh"
#include "G4AutoLock.hh"

#include <vector>

using namespace std;

namespace { G4Mutex DRsimRunActionMutex = G4MUTEX_INITIALIZER; }
DRsimRootInterface* DRsimRunAction::sRootIO = 0;
int DRsimRunAction::sNumRef = 0;
int DRsimRunAction::sNumEvt = 0;

DRsimRunAction::DRsimRunAction(int seed, G4String hepMCpath)
: G4UserRunAction()
{
  fSeed = seed;
  fHepMCpath = hepMCpath;

  G4AutoLock lock(&DRsimRunActionMutex);
  sNumRef++;
  if (!sRootIO) sRootIO = new DRsimRootInterface(fHepMCpath+"_"+std::to_string(fSeed)+".root","DRsim");
}

DRsimRunAction::~DRsimRunAction() {
  G4AutoLock lock(&DRsimRunActionMutex);
  sNumRef--;
  if (sRootIO && sNumRef==0) {
    sRootIO->close();
    delete sRootIO;
    sRootIO = 0;
  }
}

void DRsimRunAction::BeginOfRunAction(const G4Run*) {

}

void DRsimRunAction::EndOfRunAction(const G4Run*) {

}
