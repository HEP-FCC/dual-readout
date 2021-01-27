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
: G4UserRunAction()
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

    pStore = std::make_unique<podio::EventStore>();
    pWriter = std::make_unique<podio::ROOTWriter>( fFilename + "_t" + std::to_string(G4Threading::G4GetThreadId()) + ".root", pStore.get() );

    pSaveHits = std::make_unique<SimG4SaveDRcaloHits>(pStore.get(),pWriter.get());
    pEventAction->SetWriter(pWriter.get());
    pEventAction->SetEventStore(pStore.get());
    pEventAction->SetSaveHits(pSaveHits.get());
  }

  if ( G4Threading::IsMasterThread() ) {
    G4Material::GetMaterial("DR_Polystyrene")->GetIonisation()->SetBirksConstant(0.126*mm/MeV); // makeshift for DD4hep
    auto* emSaturation = G4LossTableManager::Instance()->EmSaturation();
    emSaturation->DumpBirksCoefficients();
  }
}

void DRsimRunAction::EndOfRunAction(const G4Run*) {
  if ( !IsMaster() || !G4Threading::IsMultithreadedApplication() ) {
    pWriter->finish();

    if (fOpFiberRegion) delete fOpFiberRegion;
  }
}
