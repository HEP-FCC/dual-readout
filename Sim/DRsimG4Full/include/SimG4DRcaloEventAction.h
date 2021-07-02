#ifndef SimG4DRcaloEventAction_h
#define SimG4DRcaloEventAction_h 1

#include "G4UserEventAction.hh"
#include "G4Event.hh"

#include "SimG4DRcaloSteppingAction.h"

#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/SimCalorimeterHitCollection.h"

namespace drc {
class SimG4DRcaloEventAction : public G4UserEventAction {
public:
  SimG4DRcaloEventAction();
  virtual ~SimG4DRcaloEventAction();

  virtual void BeginOfEventAction(const G4Event*) final;
  virtual void EndOfEventAction(const G4Event*) final;

  void setSteppingAction(SimG4DRcaloSteppingAction* steppingAction) { pSteppingAction = steppingAction; }

  edm4hep::SimCalorimeterHitCollection* getEdepsCollection() { return m_Edeps; }
  edm4hep::MCParticleCollection* getLeakagesCollection() { return m_Leakages; }

private:
  SimG4DRcaloSteppingAction* pSteppingAction;

  // ownership of collections transferred to DataWrapper<T>
  edm4hep::SimCalorimeterHitCollection* m_Edeps;
  edm4hep::MCParticleCollection* m_Leakages;
};
}

#endif
