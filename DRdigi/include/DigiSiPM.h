#ifndef DigiSiPM_h
#define DigiSiPM_h 1

#include "edm4hep/RawCalorimeterHitCollection.h"
#include "edm4hep/CalorimeterHitCollection.h"
#include "edm4hep/RawTimeSeriesCollection.h"
#include "edm4hep/TimeSeriesCollection.h"
#include "edm4hep/EventHeaderCollection.h"

#include "k4FWCore/DataHandle.h"

#include "Gaudi/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "SiPMSensor.h"

class DigiSiPM : public Gaudi::Algorithm {
public:
  DigiSiPM(const std::string& name, ISvcLocator* svcLoc);
  virtual ~DigiSiPM() {};

  StatusCode initialize();
  StatusCode execute(const EventContext&) const;
  StatusCode finalize();

private:
  mutable DataHandle<edm4hep::RawCalorimeterHitCollection> m_rawHits{"RawCalorimeterHits", Gaudi::DataHandle::Reader, this};
  mutable DataHandle<edm4hep::RawTimeSeriesCollection> m_timeStruct{"RawTimeStructs", Gaudi::DataHandle::Reader, this};

  mutable DataHandle<edm4hep::CalorimeterHitCollection> m_digiHits{"DigiCalorimeterHits", Gaudi::DataHandle::Writer, this};
  mutable DataHandle<edm4hep::TimeSeriesCollection> m_waveforms{"DigiWaveforms", Gaudi::DataHandle::Writer, this};

  std::unique_ptr<sipm::SiPMSensor> m_sensor;

  // Hamamatsu S14160-1310PS
  Gaudi::Property<double> m_sigLength{this, "signalLength", 200., "signal length in ns"};
  Gaudi::Property<double> m_sipmSize{this, "SiPMsize", 1.3, "width of photosensitive area in mm"};
  Gaudi::Property<double> m_Dcr{this, "DCR", 120e3, "SiPM DCR"};
  Gaudi::Property<double> m_Xt{this, "Xtalk", 0.01, "SiPM crosstalk"};
  Gaudi::Property<double> m_sampling{this, "sampling", 0.1, "SiPM sampling rate in ns"};
  Gaudi::Property<double> m_recovery{this, "recovery", 10., "SiPM cell recovery time in ns"}; // https://arxiv.org/abs/2001.10322
  Gaudi::Property<double> m_cellPitch{this, "cellpitch", 10., "SiPM cell size in um"};
  Gaudi::Property<double> m_afterpulse{this, "afterpulse", 0.03, "afterpulse probability"};
  Gaudi::Property<double> m_falltimeFast{this, "falltimeFast", 6.5, "signal fast component decay time in ns"};
  Gaudi::Property<double> m_risetime{this, "risetime", 1., "signal rise time in ns"};
  Gaudi::Property<double> m_snr{this, "SNR", 20., "SNR value in dB"};

  // integration parameters
  Gaudi::Property<double> m_gateStart{this, "gateStart", 5., "Integration gate starting time in ns"};
  Gaudi::Property<double> m_gateL{this, "gateLength", 95., "Integration gate length in ns"};  // Should be approx 5 times fallTimeFast (see above)
  Gaudi::Property<double> m_thres{this, "threshold", 1.5, "Integration threshold"};  // Threshold in pe (1.5 to suppress DCR)
};

#endif
