#ifndef DRcalib3D_h
#define DRcalib3D_h 1

#include "k4FWCore/DataHandle.h"

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "edm4hep/RawCalorimeterHitCollection.h"
#include "edm4hep/CalorimeterHitCollection.h"
#include "edm4hep/SparseVectorCollection.h"

#include "GridDRcalo.h"
#include "k4Interface/IGeoSvc.h"

class IGeoSvc;

class DRcalib3D : public GaudiAlgorithm {
public:
  DRcalib3D(const std::string& name, ISvcLocator* svcLoc);
  virtual ~DRcalib3D() {};

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  TH1* processFFT(TH1* waveHist);

private:
  ServiceHandle<IGeoSvc> m_geoSvc;
  dd4hep::DDSegmentation::GridDRcalo* pSeg;
  dd4hep::DDSegmentation::DRparamBase* pParamBase;

  DataHandle<edm4hep::RawCalorimeterHitCollection> m_digiHits{"DigiCalorimeterHits", Gaudi::DataHandle::Reader, this};
  DataHandle<edm4hep::SparseVectorCollection> m_waveforms{"DigiWaveforms", Gaudi::DataHandle::Reader, this};
  DataHandle<edm4hep::CalorimeterHitCollection> m_2dHits{"DRcalo2dHits", Gaudi::DataHandle::Reader, this};
  DataHandle<edm4hep::CalorimeterHitCollection> m_caloHits{"DRcalo3dHits", Gaudi::DataHandle::Writer, this};
  DataHandle<edm4hep::SparseVectorCollection> m_postprocTime{"DRpostprocTime", Gaudi::DataHandle::Writer, this};

  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "DRcaloSiPMreadout", "readout name of DRcalo"};

  Gaudi::Property<double> m_sampling{this, "sampling", 0.1, "SiPM sampling rate in ns"};
  Gaudi::Property<double> m_gateStart{this, "gateStart", 10., "Integration gate starting time in ns"};
  Gaudi::Property<double> m_gateL{this, "gateLength", 90., "Integration gate length in ns"};
  Gaudi::Property<double> m_zero{this, "threshold", 0.01, "FFT postprocessing threshold (ratio to the peak)"};
  Gaudi::Property<int> m_nbins{this, "nbins", 900, "number of bins for FFT"};

  Gaudi::Property<double> m_scintSpeed{this, "scintSpeed", 174.2, "effective scintillation photon speed in mm/ns"}; // candidate_1 173.5 // candidate_2 158.8
  Gaudi::Property<double> m_cherenSpeed{this, "cherenSpeed", 189.5, "effective Cherenkov photon speed in mm/ns"};
};

#endif
