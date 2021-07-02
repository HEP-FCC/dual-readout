#include "SimG4DRcaloActionInitialization.h"
#include "SimG4DRcaloSteppingAction.h"
#include "SimG4DRcaloEventAction.h"

namespace drc {
SimG4DRcaloActionInitialization::SimG4DRcaloActionInitialization(): G4VUserActionInitialization() {}

SimG4DRcaloActionInitialization::~SimG4DRcaloActionInitialization() {}

void SimG4DRcaloActionInitialization::Build() const {
  SimG4DRcaloSteppingAction* steppingAction = new SimG4DRcaloSteppingAction(); // deleted by G4
  steppingAction->setSegmentation(pSeg);
  SetUserAction(steppingAction);

  SimG4DRcaloEventAction* eventAction = new SimG4DRcaloEventAction(); // deleted by G4
  eventAction->setSteppingAction(steppingAction);
  SetUserAction(eventAction);
}
}
