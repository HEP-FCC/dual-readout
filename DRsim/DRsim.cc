#include <iostream>
#include <stdexcept>

#include "GeoSvc.h"

#include "DRsimActionInitialization.hh"

#include "G4RunManagerFactory.hh"

#include "G4UImanager.hh"
#include "G4OpticalPhysics.hh"
#include "G4OpticalParameters.hh"
#include "G4FastSimulationPhysics.hh"
#include "G4HadronicProcessStore.hh"
#include "FTFP_BERT.hh"
#include "Randomize.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

int main(int argc, char** argv) {
  // Detect interactive mode (if no arguments) and define UI session
  G4UIExecutive* ui = 0;
  if ( argc == 1 ) ui = new G4UIExecutive(argc, argv);

  G4int seed = 0;
  G4String filename;
  if (argc > 2) seed = atoi(argv[2]);
  if (argc > 3) filename = argv[3];

  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  CLHEP::HepRandom::setTheSeed(seed);

  // Construct the default run manager
  auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

  std::ifstream compacts("compacts.txt");
  if (!compacts) throw std::runtime_error("Cannot find compacts.txt!");
  std::string xmlFile = "";
  std::vector<std::string> xmlList = {};

  while (std::getline(compacts,xmlFile)) {
    if (xmlFile.size()>0) xmlList.push_back(xmlFile);
  }


  // Mandatory user initialization classes
  auto geoSvc = new GeoSvc(xmlList);

  runManager->SetUserInitialization(geoSvc->getGeant4Geo());

  // physics module
  G4VModularPhysicsList* physicsList = new FTFP_BERT;

  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  physicsList->RegisterPhysics(opticalPhysics);
  auto opticalParams = G4OpticalParameters::Instance();
  opticalParams->SetBoundaryInvokeSD(true);
  opticalParams->SetProcessActivation("Cerenkov",true);
  opticalParams->SetProcessActivation("Scintillation",true);
  opticalParams->SetCerenkovTrackSecondariesFirst(true);
  opticalParams->SetScintTrackSecondariesFirst(true);

  G4FastSimulationPhysics* fastsimPhysics = new G4FastSimulationPhysics();
  fastsimPhysics->ActivateFastSimulation("opticalphoton");
  physicsList->RegisterPhysics(fastsimPhysics);

  runManager->SetUserInitialization(physicsList);

  // User action initialization
  runManager->SetUserInitialization(new DRsimActionInitialization(seed,filename));

  // Visualization manager construction
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  G4HadronicProcessStore::Instance()->SetVerbose(0);
  UImanager->ApplyCommand("/process/em/verbose 0");

  if ( argc != 1 ) {
    // execute an argument macro file if exist
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  } else {
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    if (ui->IsGUI()) { UImanager->ApplyCommand("/control/execute gui.mac"); }
    // start interactive session
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !

  delete visManager;
  delete runManager;

  return 0;
}
