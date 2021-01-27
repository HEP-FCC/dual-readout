#ifndef DRsimRunAction_h
#define DRsimRunAction_h 1

#include "DRsimInterface.h"
#include "DRsimEventAction.hh"
#include "HepMCG4Reader.hh"

#include "SimG4FastSimOpFiberRegion.h"
#include "SimG4SaveDRcaloHits.h"

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

  void SetEventAction(DRsimEventAction* eventAction) { pEventAction = eventAction; }

  static HepMCG4Reader* sHepMCreader;
  static int sNumEvt;

private:
  G4int fSeed;
  G4String fFilename;
  G4bool fUseHepMC;
  SimG4FastSimOpFiberRegion* fOpFiberRegion;
  std::unique_ptr<podio::EventStore> pStore;
  std::unique_ptr<podio::ROOTWriter> pWriter;
  std::unique_ptr<SimG4SaveDRcaloHits> pSaveHits;
  DRsimEventAction* pEventAction;
};

#endif
