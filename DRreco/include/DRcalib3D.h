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

#include "TF1.h"

class TString;

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
  typedef std::map<float, float> timeEnergyMap;

  void add(edm4hep::DRrecoCalorimeterHit& drHit, edm4hep::CalorimeterHit& hit, const edm4hep::DRSimCalorimeterHit& input, const float calib);
  void addToTimeStruct(edm4hep::DRrecoCalorimeterHit& drHit, const edm4hep::DRSimCalorimeterHit& input, const float calib);
  edm4hep::Vector3f getPosition(dd4hep::DDSegmentation::CellID& cID);

  void processCheren(edm4hep::DRrecoCalorimeterHit& drHit, const edm4hep::DRSimCalorimeterHit& input,
                     const float calib, const dd4hep::Position& sipmPos, const dd4hep::Position& fiberUnit);
  void processScint(edm4hep::DRrecoCalorimeterHit& drHit, const edm4hep::DRSimCalorimeterHit& input,
                    const float calib, const dd4hep::Position& sipmPos, const dd4hep::Position& fiberUnit);
  float processThreshold(const edm4hep::DRSimCalorimeterHit& input, const float calib, timeEnergyMap& timeStruct_postprocess);
  float processThreshold(const double targetMax, timeEnergyMap& target_preprocess, timeEnergyMap& target_postprocess);
  bool pruneTail(const dd4hep::Position& pos, const dd4hep::Position& fiberUnit);

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
  float m_scintSpeed = 174.2; // 173.5; // 158.8 with raw time structure
  float m_cherenSpeed = 189.5;

  float m_minCheren = 2.; // minimum # of p.e. to process time structure
  float m_minScint = 5.; // minimum # of p.e. to process time structure
  float m_ampThres = 0.05; // minimum threshold to process

  std::unique_ptr<TF1> m_scintFit; // scintillation channel fit function

  // fit function formula in ROOT TF1 format (WARNING x in [ns]!)
  std::string m_funcFormula = "(x>[0]) ? [2]*exp(-[1]*(x-[0])) : 0"; // "([3]*[2]/2.)*exp((2*[0]+[2]*[1]*[1]-2*x)*[2]/2.)*(1-TMath::Erf(([0]+[2]*[1]*[1]-x)/(1.414*[1])))";
  double m_fitParam0 = 17.3; // target parameter of the transform
  double m_fitParam1 = 0.3; // 0.4943;
  double m_fitParam2 = 1.; // 0.2958;
  // double m_fitParam3 = 1.; // can be arbitrary (amplitude)
  double m_fitRangeLower = 10.; // lower bound of fit function
  double m_fitRangeUpper = 70.; // upper bound of fit function
  double m_svdTolerance = 0.05; // tolerance of SVD decomposition (ratio to kernel peak)
  double m_pruneTolerance = 300.; // tolerance of pruning RecHits outside the tower in [mm]

  std::string m_filenameIn;
  std::string m_filenameOut;

  std::vector<std::pair<float,float>> m_calibs;

  bool isFirstEvent = true;
  int verbose = 0;
};

#endif
