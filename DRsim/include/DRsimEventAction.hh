#ifndef DRsimEventAction_h
#define DRsimEventAction_h 1

#include "DRsimInterface.h"
#include "SimG4SaveDRcaloHits.h"

#include "G4UserEventAction.hh"
#include "G4HCofThisEvent.hh"
#include "G4Event.hh"

class DRsimEventAction : public G4UserEventAction {
public:
  typedef std::pair<int,int> toweriTiP;

  DRsimEventAction();
  virtual ~DRsimEventAction();

  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);

  void SetSaveHits(SimG4SaveDRcaloHits* saveHits) { pSaveHits = saveHits; }
  void SetEventStore(podio::EventStore* theStore) { pStore = theStore; }
  void SetWriter(podio::ROOTWriter* theWriter) { pWriter = theWriter; }

  DRsimInterface::DRsimEventData* getEventData() { return fEventData; }

private:
  void clear();
  void fillPtcs(G4PrimaryVertex* vtx, G4PrimaryParticle* ptc);
  void queue();

  void writeEvent() const { pWriter->writeEvent(); }
  void clearCollections() const { pStore->clearCollections(); }

  SimG4SaveDRcaloHits* pSaveHits;
  podio::EventStore* pStore;
  podio::ROOTWriter* pWriter;

  DRsimInterface::DRsimEventData* fEventData;
};

#endif
