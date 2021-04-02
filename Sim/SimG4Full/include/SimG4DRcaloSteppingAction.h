#ifndef SimG4DRcaloSteppingAction_h
#define SimG4DRcaloSteppingAction_h 1

#include "GeoSvc.h"
#include "GridDRcalo.h"

#include "G4UserSteppingAction.hh"

#include "podio/ROOTWriter.h"
#include "podio/EventStore.h"

// Data model
#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/SimCalorimeterHitCollection.h"

class SimG4DRcaloSteppingAction : public G4UserSteppingAction {
public:
  SimG4DRcaloSteppingAction();
  virtual ~SimG4DRcaloSteppingAction();

  virtual void UserSteppingAction(const G4Step*);

  void initialize();
  void initializeEDM();

  void setEventData(DRsimInterface::DRsimEventData* evtData) { fEventData = evtData; }

  void SetEventStore(podio::EventStore* theStore) { pStore = theStore; }
  void SetWriter(podio::ROOTWriter* theWriter) { pWriter = theWriter; }

private:
  template <typename T>
  void accumulate(unsigned int &prev, long long int id64, float edep);
  bool checkId(edm4hep::SimCalorimeterHit edep, long long int id64);

  void saveLeakage(G4Track* track, G4StepPoint* pre);

  /// Pointer to the geometry service
  GeoSvc* m_geoSvc;

  DRsimInterface::DRsimEventData* fEventData;
  dd4hep::DDSegmentation::GridDRcalo* fSeg;

  /// Name of the readout to save
  std::string m_readoutName;

  unsigned int fPrevTower;
  unsigned int fPrevFiber;

  podio::EventStore* pStore;
  podio::ROOTWriter* pWriter;

  edm4hep::MCParticleCollection* mLeakages;
  edm4hep::SimCalorimeterHitCollection* mEdeps;
};

#endif
