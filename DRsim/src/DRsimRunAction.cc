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
RootInterface<DRsimInterface::DRsimEventData>* DRsimRunAction::sRootIO = 0;
int DRsimRunAction::sNumEvt = 0;

DRsimRunAction::DRsimRunAction(G4int seed, G4String filename, G4bool useHepMC)
: G4UserRunAction()
{
  fSeed = seed;
  fFilename = filename;
  fUseHepMC = useHepMC;

  G4AutoLock lock(&DRsimRunActionMutex);

  if (!sRootIO) {
    sRootIO = new RootInterface<DRsimInterface::DRsimEventData>(fFilename+"_"+std::to_string(fSeed)+".root");
    sRootIO->create("DRsim","DRsimEventData");
  }

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

    if (sRootIO) {
      sRootIO->write();
      sRootIO->close();
      delete sRootIO;
      sRootIO = 0;
    }
  }
}

void DRsimRunAction::BeginOfRunAction(const G4Run*) {
  // Mimic SimG4Svc
  if (!IsMaster()) {
    fOpFiberRegion = new SimG4FastSimOpFiberRegion();
    fOpFiberRegion->create();
  }

  if ( G4Threading::IsMasterThread() ) {
    G4Material::GetMaterial("DR_Polystyrene")->GetIonisation()->SetBirksConstant(0.126*mm/MeV); // makeshift for DD4hep #TODO ask DD4hep authors to implement SetBirksConstant()
    auto* emSaturation = G4LossTableManager::Instance()->EmSaturation();
    emSaturation->DumpBirksCoefficients();
  }
}

void DRsimRunAction::EndOfRunAction(const G4Run*) {
  if (!IsMaster()) {
    if (fOpFiberRegion) delete fOpFiberRegion;
  }
}
