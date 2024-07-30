#ifndef SimG4SaveDRcaloHits_h
#define SimG4SaveDRcaloHits_h 1

#include "DRcaloSiPMHit.h"

// Data model
#include "edm4hep/RawCalorimeterHitCollection.h"
#include "edm4hep/RawTimeSeriesCollection.h"

#include "GaudiKernel/AlgTool.h"
#include "k4FWCore/DataHandle.h"
#include "k4Interface/ISimG4SaveOutputTool.h"
#include "k4Interface/IGeoSvc.h"

class IGeoSvc;

class SimG4SaveDRcaloHits : public AlgTool, virtual public ISimG4SaveOutputTool {
public:
  explicit SimG4SaveDRcaloHits(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~SimG4SaveDRcaloHits();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode saveOutput(const G4Event& aEvent) final;

private:
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;

  Gaudi::Property<std::vector<std::string>> m_readoutNames{this, "readoutNames", {"DRcaloSiPMreadout"}, "Name of the readouts (hits collections) to save"};

  mutable DataHandle<edm4hep::RawCalorimeterHitCollection> mRawCaloHits{"RawCalorimeterHits", Gaudi::DataHandle::Writer, this};
  mutable DataHandle<edm4hep::RawTimeSeriesCollection> mTimeStruct{"RawTimeStructs", Gaudi::DataHandle::Writer, this};
  mutable DataHandle<edm4hep::RawTimeSeriesCollection> mWavlenStruct{"RawWavlenStructs", Gaudi::DataHandle::Writer, this};
};

#endif
