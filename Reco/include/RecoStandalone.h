#ifndef RecoStandalone_h
#define RecoStandalone_h 1

#include "edm4hep/RawCalorimeterHitCollection.h"
#include "edm4hep/DRSimCalorimeterHitCollection.h"
#include "edm4hep/CalorimeterHitCollection.h"
#include "edm4hep/DRrecoCalorimeterHitCollection.h"
#include "edm4hep/DRrecoCaloAssociationCollection.h"
#include "edm4hep/SimCalorimeterHitCollection.h"
#include "edm4hep/EventHeaderCollection.h"

#include "GridDRcalo.h"
#include "podio/ROOTWriter.h"
#include "podio/ROOTReader.h"
#include "GeoSvc.h"

class RecoStandalone {
public:
  RecoStandalone(const std::string filenameIn, const std::string filenameOut);
  virtual ~RecoStandalone() {};

  void initialize();
  void execute();
  void finalize();

  podio::ROOTReader* GetReader() { return m_reader.get(); }
  podio::ROOTWriter* GetWriter() { return m_writer.get(); }
  podio::EventStore* GetStore() { return m_store.get(); }

  void readCSV(std::string filename);

private:
  void add(edm4hep::DRrecoCalorimeterHit& drHit, edm4hep::CalorimeterHit& hit, const edm4hep::DRSimCalorimeterHit& input,
           const std::vector<float>& timeBinCenter, float calib);
  void addToTimeStruct(edm4hep::DRrecoCalorimeterHit& drHit, const edm4hep::DRSimCalorimeterHit& input,
                       const std::vector<float>& timeBinCenter, float calib);
  edm4hep::Vector3f getPosition(dd4hep::DDSegmentation::CellID& cID);

  template <typename T>
  void create(T*& pCollection, std::string name);

  template <typename T>
  void registerForWrite(T*& pCollection, std::string name);

  GeoSvc* m_geoSvc;
  dd4hep::DDSegmentation::GridDRcalo* pSeg;
  dd4hep::DDSegmentation::DRparamBase* pParamBase;

  std::unique_ptr<podio::ROOTReader> m_reader;
  std::unique_ptr<podio::ROOTWriter> m_writer;
  std::unique_ptr<podio::EventStore> m_store;

  edm4hep::CalorimeterHitCollection* m_sHits;
  edm4hep::CalorimeterHitCollection* m_cHits;
  edm4hep::DRrecoCalorimeterHitCollection* m_DRScintHits;
  edm4hep::DRrecoCalorimeterHitCollection* m_DRCherenHits;
  edm4hep::DRrecoCaloAssociationCollection* m_scintAssocs;
  edm4hep::DRrecoCaloAssociationCollection* m_cherenAssocs;

  std::string m_readoutName = "DRcaloSiPMreadout";
  std::string m_calibPath = "calib.csv";
  float m_scintSpeed = 158.8;
  float m_cherenSpeed = 189.5;

  std::string m_filenameIn;
  std::string m_filenameOut;

  std::vector<std::pair<float,float>> m_calibs;

  bool isFirstEvent = true;
};

#endif
