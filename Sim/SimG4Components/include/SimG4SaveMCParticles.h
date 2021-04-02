#ifndef SimG4SaveMCParticles_h
#define SimG4SaveMCParticles_h 1

#include "GeoSvc.h"

#include "podio/ROOTWriter.h"
#include "podio/EventStore.h"

// Data model
#include "edm4hep/MCParticleCollection.h"

class SimG4SaveMCParticles {
public:
  explicit SimG4SaveMCParticles(podio::EventStore* store, podio::ROOTWriter* writer);
  virtual ~SimG4SaveMCParticles();

  void initialize();

  void saveOutput(const G4Event* aEvent) const;

private:
  podio::EventStore* pStore;
  podio::ROOTWriter* pWriter;

  edm4hep::MCParticleCollection* mMCparticles;
};

#endif
