#include "DRsimRunAction.hh"
#include "DRsimEventAction.hh"
#include "G4AutoLock.hh"
#include "G4Threading.hh"

#include "G4EmSaturation.hh"
#include "G4LossTableManager.hh"
#include "G4Material.hh"
#include "G4SystemOfUnits.hh"

#include <vector>

using namespace std;

namespace { G4Mutex DRsimRunActionMutex = G4MUTEX_INITIALIZER; }
HepMCG4Reader* DRsimRunAction::sHepMCreader = 0;
int DRsimRunAction::sNumEvt = 0;

DRsimRunAction::DRsimRunAction(G4int seed, G4String filename, G4bool useHepMC)
: pSaveHits(nullptr), G4UserRunAction()
{
  fSeed = seed;
  fFilename = filename;
  fUseHepMC = useHepMC;

  G4AutoLock lock(&DRsimRunActionMutex);

  if (fUseHepMC && !sHepMCreader) {
    sHepMCreader = new HepMCG4Reader(fSeed,fFilename); // activated in the worker node (by macro command)
  }
}

DRsimRunAction::~DRsimRunAction() {
  if (IsMaster()) {
    G4AutoLock lock(&DRsimRunActionMutex);

    if (fUseHepMC && sHepMCreader) {
      delete sHepMCreader;
      sHepMCreader = 0;
    }
  }
}

void DRsimRunAction::BeginOfRunAction(const G4Run*) {
  // Mimic SimG4Svc
  if ( !IsMaster() || !G4Threading::IsMultithreadedApplication() ) {
    fOpFiberRegion = new SimG4FastSimOpFiberRegion();
    fOpFiberRegion->create();

    pSaveHits = std::make_unique<SimG4SaveDRcaloHits>( fFilename + "_t" + std::to_string(G4Threading::G4GetThreadId()) + ".root" );
  }

  if ( G4Threading::IsMasterThread() ) {
    G4Material::GetMaterial("DR_Polystyrene")->GetIonisation()->SetBirksConstant(0.126*mm/MeV); // makeshift for DD4hep
    auto* emSaturation = G4LossTableManager::Instance()->EmSaturation();
    emSaturation->DumpBirksCoefficients();
  }
}

void DRsimRunAction::EndOfRunAction(const G4Run*) {
  if ( !IsMaster() || !G4Threading::IsMultithreadedApplication() ) {
    pSaveHits.reset();

    if (fOpFiberRegion) delete fOpFiberRegion;
  }
}
