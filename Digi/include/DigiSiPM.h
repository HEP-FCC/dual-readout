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

  // Hamamatsu S14160-1310PS
  Gaudi::property<double> m_sigLength{this, "signalLength", 200., "signal length in ns"};
  Gaudi::property<double> m_sipmSize{this, "SiPMsize", 1.3, "width of photosensitive area in mm"};
  Gaudi::Property<double> m_Dcr{this, "DCR", 120e3, "SiPM DCR"};
  Gaudi::Property<double> m_Xt{this, "Xtalk", 0.01, "SiPM crosstalk"};
  Gaudi::Property<double> m_sampling{this, "sampling", 0.1, "SiPM sampling rate in ns"};
  Gaudi::Property<double> m_recovery{this, "recovery", 10., "SiPM cell recovery time in ns"}; // https://arxiv.org/abs/2001.10322
  Gaudi::Property<double> m_cellPitch{this, "cellpitch", 10., "SiPM cell size in um"};
  Gaudi::Property<double> m_afterpulse{this, "afterpulse", 0.03, "afterpulse probability"};
  Gaudi::Property<double> m_falltimeFast{this, "falltimeFast", 6.5, "signal fast component decay time in ns"};
  Gaudi::Property<double> m_risetime{this, "risetime", 1., "signal rise time in ns"};
  Gaudi::Property<double> m_snr{this, "SNR", 30., "SNR value in dB"};

  // ADC parameters
  Gaudi::Property<unsigned int> m_bits{this, "bits", 14, "ADC bits"};
  Gaudi::Property<double> m_range{this, "range", std::pow(2., 13.), "ADC output range"};
  Gaudi::Property<double> m_gain{this, "gain", 13.979, "ADC gain in dB i.e. gain(linear) = 10^(m_gain/20)"};
  // reciprocal_width (multiplication factor) = 1. / (range / gain(linear) / 2^bits);

  // integration parameters
  Gaudi::Property<double> m_gateStart{this, "gateStart", 10., "Integration gate starting time in ns"};
  Gaudi::Property<double> m_gateL{this, "gateLength", 90., "Integration gate length in ns"};
  Gaudi::Property<int> m_thres{this, "threshold", static_cast<int>( 1.5 * 10. ), "Integration threshold"};
};

#endif
