#include "DRsimRunAction.hh"
#include "DRsimEventAction.hh"

#include <vector>
using namespace std;

DRsimRunAction::DRsimRunAction(int seed, G4int wavBin, G4int timeBin)
: G4UserRunAction()
{
  fSeed = seed;
  fWavBin = wavBin;
  fTimeBin = timeBin;

  // char outputname[128]; std::sprintf(outputname,"dr_seednum%d",fSeed);
}

DRsimRunAction::~DRsimRunAction() {}

void DRsimRunAction::BeginOfRunAction(const G4Run*) {

}

void DRsimRunAction::EndOfRunAction(const G4Run*) {
  
}
