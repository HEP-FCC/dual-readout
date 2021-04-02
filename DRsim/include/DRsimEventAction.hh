#ifndef DRsimEventAction_h
#define DRsimEventAction_h 1

#include "SimG4SaveDRcaloHits.h"
#include "SimG4SaveMCParticles.h"

#include "G4UserEventAction.hh"
#include "G4HCofThisEvent.hh"
#include "G4Event.hh"

// Data model
#include "edm4hep/EventHeaderCollection.h"

class DRsimEventAction : public G4UserEventAction {
public:
  typedef std::pair<int,int> toweriTiP;

  DRsimEventAction();
  virtual ~DRsimEventAction();

  void initialize();

  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);

  void SetSaveHits(SimG4SaveDRcaloHits* saveHits) { pSaveHits = saveHits; }
  void SetSaveMCParticles(SimG4SaveMCParticles* saveMCParticles) { pSaveMCParticles = saveMCParticles; }
  void SetEventStore(podio::EventStore* theStore) { pStore = theStore; }
  void SetWriter(podio::ROOTWriter* theWriter) { pWriter = theWriter; }
  void SetRunNumber(int runNumber) { mRunNumber = runNumber; }

private:
  void writeEvent() const { pWriter->writeEvent(); }
  void clearCollections() const { pStore->clearCollections(); }

  SimG4SaveDRcaloHits* pSaveHits;
  SimG4SaveMCParticles* pSaveMCParticles;
  podio::EventStore* pStore;
  podio::ROOTWriter* pWriter;

  edm4hep::EventHeaderCollection* mEvtHeaders;
  int mRunNumber;
};

#endif
