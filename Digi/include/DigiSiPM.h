#ifndef DigiSiPM_h
#define DigiSiPM_h 1

#include "edm4hep/RawCalorimeterHitCollection.h"
#include "edm4hep/SparseVectorCollection.h"
#include "edm4hep/EventHeaderCollection.h"

#include "k4FWCore/DataHandle.h"

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "SiPMSensor.h"
#include "SiPMAdc.h"

class DigiSiPM : public GaudiAlgorithm {
public:
  DigiSiPM(const std::string& name, ISvcLocator* svcLoc);
  virtual ~DigiSiPM() {};

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  DataHandle<edm4hep::RawCalorimeterHitCollection> m_rawHits{"RawCalorimeterHits", Gaudi::DataHandle::Reader, this};
  DataHandle<edm4hep::SparseVectorCollection> m_timeStruct{"RawTimeStructs", Gaudi::DataHandle::Reader, this};

  DataHandle<edm4hep::RawCalorimeterHitCollection> m_digiHits{"DigiCalorimeterHits", Gaudi::DataHandle::Writer, this};
  DataHandle<edm4hep::SparseVectorCollection> m_waveforms{"DigiWaveforms", Gaudi::DataHandle::Writer, this};

  std::unique_ptr<sipm::SiPMSensor> m_sensor;
  std::unique_ptr<sipm::SiPMAdc> m_adc;

  // Hamamatsu S13615-1025
  Gaudi::Property<double> m_Dcr{this, "DCR", 100e3, "SiPM DCR"};
  Gaudi::Property<double> m_Xt{this, "Xtalk", 0.01, "SiPM crosstalk"};
  Gaudi::Property<double> m_sampling{this, "sampling", 0.1, "SiPM sampling rate in ns"};
  Gaudi::Property<double> m_recovery{this, "recovery", 20., "SiPM cell recovery time in ns"};

  // ADC parameters
  Gaudi::Property<unsigned int> m_bits{this, "bits", 16, "ADC bits"};
  Gaudi::Property<double> m_range{this, "range", std::pow(2., 15.), "ADC output range"};
  Gaudi::Property<double> m_gain{this, "gain", 20., "ADC gain in dB i.e. gain(linear) = 10^(m_gain/20)"};

  // integration parameters
  Gaudi::Property<double> m_gateStart{this, "gateStart", 10., "Integration gate starting time in ns"};
  Gaudi::Property<double> m_gateL{this, "gateLength", 250., "Integration gate length in ns"};
  Gaudi::Property<int> m_thres{this, "threshold", static_cast<int>( 0.5 * (std::pow(10.,20./20.) * std::pow(2., 16.) / std::pow(2., 15.) ) ), "Integration threshold"};
};

#endif
