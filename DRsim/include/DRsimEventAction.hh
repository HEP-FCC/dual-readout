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

  DRsimInterface::DRsimEventData* getEventData() { return fEventData; }

private:
  void clear();
  void fillPtcs(G4PrimaryVertex* vtx, G4PrimaryParticle* ptc);
  void queue();

  SimG4SaveDRcaloHits* fSaveHits;

  DRsimInterface::DRsimEventData* fEventData;
};

#endif
