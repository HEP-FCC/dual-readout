#ifndef SimG4DRcaloSteppingAction_h
#define SimG4DRcaloSteppingAction_h 1

#include "GridDRcalo.h"

#include "G4UserSteppingAction.hh"
#include "G4Track.hh"
#include "G4StepPoint.hh"

// Data model
#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/SimCalorimeterHitCollection.h"

namespace drc {
class SimG4DRcaloSteppingAction : public G4UserSteppingAction {
public:
  SimG4DRcaloSteppingAction();
  virtual ~SimG4DRcaloSteppingAction();

  virtual void UserSteppingAction(const G4Step*);

  void setSegmentation(dd4hep::DDSegmentation::GridDRcalo* seg) { pSeg = seg; }
  void setEdepsCollection(edm4hep::SimCalorimeterHitCollection* data) { m_Edeps = data; }
  void setEdeps3dCollection(edm4hep::SimCalorimeterHitCollection* data) { m_Edeps3d = data; }
  void setLeakagesCollection(edm4hep::MCParticleCollection* data) { m_Leakages = data; }

  void setThreshold(const double thres) { m_thres = thres; }

private:
  void accumulate(unsigned int &prev, dd4hep::DDSegmentation::CellID& id64, float edep);
  bool checkId(edm4hep::SimCalorimeterHit edep, dd4hep::DDSegmentation::CellID& id64);

  void saveLeakage(G4Track* track, G4StepPoint* pre);

  unsigned int fPrevTower;
  unsigned int fPrevFiber;

  dd4hep::DDSegmentation::GridDRcalo* pSeg;

  // collections owned by SimG4DRcaloEventAction
  edm4hep::SimCalorimeterHitCollection* m_Edeps;
  edm4hep::SimCalorimeterHitCollection* m_Edeps3d;
  edm4hep::MCParticleCollection* m_Leakages;
  double m_thres;
};
}

#endif
