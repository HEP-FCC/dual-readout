#include "SimG4OpticalPhysicsList.h"

// Geant4
#include "G4VModularPhysicsList.hh"
#include "G4OpticalPhysics.hh"
#include "G4OpticalParameters.hh"

DECLARE_COMPONENT(SimG4OpticalPhysicsList)

SimG4OpticalPhysicsList::SimG4OpticalPhysicsList(const std::string& aType, const std::string& aName, const IInterface* aParent)
: AlgTool(aType, aName, aParent) {
  declareInterface<ISimG4PhysicsList>(this);
  declareProperty("fullphysics", m_physicsListTool, "Handle for the full physics list tool");
}

SimG4OpticalPhysicsList::~SimG4OpticalPhysicsList() {}

StatusCode SimG4OpticalPhysicsList::initialize() {
  if (AlgTool::initialize().isFailure())
    return StatusCode::FAILURE;

  m_physicsListTool.retrieve().ignore();

  return StatusCode::SUCCESS;
}

StatusCode SimG4OpticalPhysicsList::finalize() { return AlgTool::finalize(); }

G4VModularPhysicsList* SimG4OpticalPhysicsList::physicsList() {
  // ownership passed to SimG4Svc which will register it in G4RunManager. To be deleted in ~G4RunManager()
  G4VModularPhysicsList* physicsList = m_physicsListTool->physicsList();

  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics(); // deleted in ~G4VModularPhysicsList()
  physicsList->RegisterPhysics(opticalPhysics);

  auto* opticalParams = G4OpticalParameters::Instance();
  opticalParams->SetBoundaryInvokeSD(true);
  opticalParams->SetProcessActivation("Cerenkov",true);
  opticalParams->SetProcessActivation("Scintillation",true);
  opticalParams->SetCerenkovTrackSecondariesFirst(true);
  opticalParams->SetScintTrackSecondariesFirst(true);

  return physicsList;
}
