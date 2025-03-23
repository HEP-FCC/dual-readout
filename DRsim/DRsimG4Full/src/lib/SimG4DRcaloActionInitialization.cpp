#include "SimG4DRcaloActionInitialization.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "SimG4DRcaloEventAction.h"
#include "SimG4DRcaloSteppingAction.h"

namespace drc {
SimG4DRcaloActionInitialization::SimG4DRcaloActionInitialization() : G4VUserActionInitialization() {}

SimG4DRcaloActionInitialization::~SimG4DRcaloActionInitialization() {}

void SimG4DRcaloActionInitialization::setBirksConstant(const std::string scintName, const double birks) {
  m_scintName = scintName;
  m_birks = birks;
}

void SimG4DRcaloActionInitialization::Build() const {
  SimG4DRcaloSteppingAction* steppingAction = new SimG4DRcaloSteppingAction(); // deleted by G4
  steppingAction->setSegmentation(pSeg);
  steppingAction->setThreshold(m_thres);
  SetUserAction(steppingAction);

  SimG4DRcaloEventAction* eventAction = new SimG4DRcaloEventAction(); // deleted by G4
  eventAction->setSteppingAction(steppingAction);
  SetUserAction(eventAction);

  G4Material::GetMaterial(m_scintName)
      ->GetIonisation()
      ->SetBirksConstant(m_birks * CLHEP::millimeter / CLHEP::MeV); // makeshift for DD4hep
}
} // namespace drc
