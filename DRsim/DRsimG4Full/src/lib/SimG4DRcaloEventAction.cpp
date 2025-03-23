#include "SimG4DRcaloEventAction.h"

#include "G4RunManager.hh"

namespace drc {
SimG4DRcaloEventAction::SimG4DRcaloEventAction() : G4UserEventAction() {}

SimG4DRcaloEventAction::~SimG4DRcaloEventAction() {}

void SimG4DRcaloEventAction::BeginOfEventAction(const G4Event*) {
  m_Edeps = new edm4hep::SimCalorimeterHitCollection();
  m_Edeps3d = new edm4hep::SimCalorimeterHitCollection();
  m_Leakages = new edm4hep::MCParticleCollection();

  pSteppingAction->setEdepsCollection(m_Edeps);
  pSteppingAction->setEdeps3dCollection(m_Edeps3d);
  pSteppingAction->setLeakagesCollection(m_Leakages);

  return;
}

void SimG4DRcaloEventAction::EndOfEventAction(const G4Event*) {}
} // namespace drc
