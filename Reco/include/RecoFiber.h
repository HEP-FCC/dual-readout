#ifndef RecoFiber_h
#define RecoFiber_h 1

#include "k4FWCore/DataHandle.h"

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "edm4hep/RawCalorimeterHitCollection.h"
#include "edm4hep/DRSimCalorimeterHitCollection.h"
#include "edm4hep/CalorimeterHitCollection.h"
#include "edm4hep/DRrecoCalorimeterHitCollection.h"
#include "edm4hep/DRrecoCaloAssociationCollection.h"
#include "edm4hep/SimCalorimeterHitCollection.h"

#include "GridDRcalo.h"
#include "k4Interface/IGeoSvc.h"

class IGeoSvc;

class RecoFiber : public GaudiAlgorithm {
public:
  RecoFiber(const std::string& name, ISvcLocator* svcLoc);
  virtual ~RecoFiber() {};

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  void readCSV(std::string filename);

private:
  void add(edm4hep::DRrecoCalorimeterHit& drHit, edm4hep::CalorimeterHit& hit, const edm4hep::DRSimCalorimeterHit& input,
           const std::vector<float>& timeBinCenter, float calib);
  void addToTimeStruct(edm4hep::DRrecoCalorimeterHit& drHit, const edm4hep::DRSimCalorimeterHit& input,
                       const std::vector<float>& timeBinCenter, float calib);
  edm4hep::Vector3f getPosition(dd4hep::DDSegmentation::CellID& cID);

  ServiceHandle<IGeoSvc> m_geoSvc;
  ServiceHandle<IDataProviderSvc> m_dataSvc;
  dd4hep::DDSegmentation::GridDRcalo* pSeg;
  dd4hep::DDSegmentation::DRparamBase* pParamBase;

  /// Input collection
  DataHandle<edm4hep::DRSimCalorimeterHitCollection> m_DRsimHits{"DRSimCalorimeterHits", Gaudi::DataHandle::Reader, this};
  DataHandle<edm4hep::RawCalorimeterHitCollection> m_rawHits{"RawCalorimeterHits", Gaudi::DataHandle::Reader, this};
  DataHandle<edm4hep::SimCalorimeterHitCollection> m_simHits{"SimCalorimeterHits", Gaudi::DataHandle::Reader, this};
  // std::unique_ptr<podio::ColMDMap> m_colMDs; // #FIXME read timing bins from podio metadata
  /// Output collection
  DataHandle<edm4hep::CalorimeterHitCollection> m_sHits{"ScintillationHits", Gaudi::DataHandle::Writer, this};
  DataHandle<edm4hep::CalorimeterHitCollection> m_cHits{"CherenkovHits", Gaudi::DataHandle::Writer, this};
  DataHandle<edm4hep::DRrecoCalorimeterHitCollection> m_DRScintHits{"DRrecoScintillationHits", Gaudi::DataHandle::Writer, this};
  DataHandle<edm4hep::DRrecoCalorimeterHitCollection> m_DRCherenHits{"DRrecoCherenkovHits", Gaudi::DataHandle::Writer, this};
  DataHandle<edm4hep::DRrecoCaloAssociationCollection> m_scintAssocs{"DRscintAssociations", Gaudi::DataHandle::Writer, this};
  DataHandle<edm4hep::DRrecoCaloAssociationCollection> m_cherenAssocs{"DRcherenAssociations", Gaudi::DataHandle::Writer, this};

  Gaudi::Property<std::string> m_calibPath{this, "calibPath", "calib.csv", "relative path to calibration csv file"};
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "DRcaloSiPMreadout", "readout name of DRcalo"};

  Gaudi::Property<float> m_scintSpeed{this, "scintSpeed", 158.8, "effective photon propagation speed inside scintillation channel in mm/ns"};
  Gaudi::Property<float> m_cherenSpeed{this, "cherenSpeed", 189.5, "effective photon propagation speed inside Cherenkov channel in mm/ns"};

  std::vector<std::pair<float,float>> m_calibs;
};

#endif
