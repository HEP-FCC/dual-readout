#ifndef SimG4SaveDRcaloHits_h
#define SimG4SaveDRcaloHits_h 1

#include "GeoSvc.h"

#include "DRcaloSiPMHit.h"

#include "podio/ROOTWriter.h"
#include "podio/EventStore.h"

// Data model
#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/SimCalorimeterHitCollection.h"
#include "edm4hep/DRSimCalorimeterHitCollection.h"

class SimG4SaveDRcaloHits {
public:
  explicit SimG4SaveDRcaloHits(const std::string filename);
  virtual ~SimG4SaveDRcaloHits();

  void initialize();
  void finalize();

  const podio::EventStore* GetEventStore() { return pStore.get(); }
  const podio::ROOTWriter* GetWriter() { return pWriter.get(); }

  void saveOutput(const G4Event* aEvent) const;
  void writeEvent() const { pWriter->writeEvent(); }
  void clearCollections() const { pStore->clearCollections(); }

private:
  void addStruct( const std::map< std::pair<float,float>, int >& structData, std::function<void(int)> addTo ) const;
  void checkMetadata(const ddDRcalo::DRcaloSiPMHit* hit) const;

  /// Pointer to the geometry service
  GeoSvc* m_geoSvc;

  /// Name of the readouts (hits collections) to save
  std::vector<std::string> m_readoutNames;
  std::string mFilename;

  std::unique_ptr<podio::EventStore> pStore;
  std::unique_ptr<podio::ROOTWriter> pWriter;

  edm4hep::SimCalorimeterHitCollection* mSimCaloHits;
  edm4hep::DRSimCalorimeterHitCollection* mDRsimCaloHits;
};

#endif
