#ifndef DRsimRunAction_h
#define DRsimRunAction_h 1

#include "DRsimEventAction.hh"
#include "HepMCG4Reader.hh"

#include "SimG4FastSimOpFiberRegion.h"
#include "SimG4SaveDRcaloHits.h"
#include "SimG4SaveMCParticles.h"
#include "SimG4DRcaloSteppingAction.h"

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

class DRsimRunAction : public G4UserRunAction {
public:
  DRsimRunAction(G4int seed, G4String filename, G4bool useHepMC);
  virtual ~DRsimRunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

  const podio::EventStore* GetEventStore() { return pStore.get(); }
  const podio::ROOTWriter* GetWriter() { return pWriter.get(); }

  const SimG4SaveDRcaloHits* GetSaveHits() const { return pSaveHits.get(); }
  const SimG4SaveMCParticles* GetSaveMCParticles() const { return pSaveMCParticles.get(); }

  void SetEventAction(DRsimEventAction* eventAction) { pEventAction = eventAction; }
  void SetSteppingAction(SimG4DRcaloSteppingAction* steppingAction) { pSteppingAction = steppingAction; }

  static HepMCG4Reader* sHepMCreader;
  static int sNumEvt;

private:
  G4int fSeed;
  G4String fFilename;
  G4bool fUseHepMC;
  std::unique_ptr<SimG4FastSimOpFiberRegion> pOpFiberRegion;
  std::unique_ptr<podio::EventStore> pStore;
  std::unique_ptr<podio::ROOTWriter> pWriter;
  std::unique_ptr<SimG4SaveDRcaloHits> pSaveHits;
  std::unique_ptr<SimG4SaveMCParticles> pSaveMCParticles;
  DRsimEventAction* pEventAction;
  SimG4DRcaloSteppingAction* pSteppingAction;
};

#endif
