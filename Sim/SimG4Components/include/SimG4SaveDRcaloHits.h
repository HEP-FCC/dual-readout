#ifndef SimG4SaveDRcaloHits_h
#define SimG4SaveDRcaloHits_h 1

#include "GeoSvc.h"

#include "DRcaloSiPMHit.h"

#include "podio/ROOTWriter.h"
#include "podio/EventStore.h"

// Data model
#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/RawCalorimeterHitCollection.h"
#include "edm4hep/DRSimCalorimeterHitCollection.h"

class SimG4SaveDRcaloHits {
public:
  explicit SimG4SaveDRcaloHits(podio::EventStore* store, podio::ROOTWriter* writer);
  virtual ~SimG4SaveDRcaloHits();

  void initialize();

  void saveOutput(const G4Event* aEvent) const;

private:
  void addStruct( const std::map< std::pair<float,float>, int >& structData, std::function<void(int)> addTo ) const;
  void checkMetadata(const ddDRcalo::DRcaloSiPMHit* hit) const;

  /// Pointer to the geometry service
  GeoSvc* m_geoSvc;

  /// Name of the readouts (hits collections) to save
  std::vector<std::string> m_readoutNames;

  podio::EventStore* pStore;
  podio::ROOTWriter* pWriter;

  edm4hep::RawCalorimeterHitCollection* mRawCaloHits;
  edm4hep::DRSimCalorimeterHitCollection* mDRsimCaloHits;
};

#endif
