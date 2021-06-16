#ifndef DigiPodio_h
#define DigiPodio_h 1

#include "edm4hep/RawCalorimeterHitCollection.h"
#include "edm4hep/DRSimCalorimeterHitCollection.h"
#include "edm4hep/SimCalorimeterHitCollection.h"
#include "edm4hep/EventHeaderCollection.h"

#include "podio/ROOTWriter.h"
#include "podio/ROOTReader.h"

#include "SiPMSensor.h"
#include "SiPMAdc.h"

namespace drc {

class DigiPodio {
public:
  DigiPodio(const std::string filenameIn, const std::string filenameOut);
  virtual ~DigiPodio() {};

  void initialize();
  void execute();
  void finalize();

  podio::ROOTReader* GetReader() { return m_reader.get(); }
  podio::ROOTWriter* GetWriter() { return m_writer.get(); }
  podio::EventStore* GetStore() { return m_store.get(); }

private:
  template <typename T>
  void create(T*& pCollection, std::string name);

  template <typename T>
  void registerForWrite(T*& pCollection, std::string name);

  std::unique_ptr<podio::ROOTReader> m_reader;
  std::unique_ptr<podio::ROOTWriter> m_writer;
  std::unique_ptr<podio::EventStore> m_store;

  edm4hep::RawCalorimeterHitCollection* m_digiHits;
  edm4hep::DRSimCalorimeterHitCollection* m_DRdigiHits;

  std::string m_filenameIn;
  std::string m_filenameOut;

  std::unique_ptr<sipm::SiPMSensor> m_sensor;
  std::unique_ptr<sipm::SiPMAdc> m_adc;

  // Hamamatsu S13615-1025
  double m_Dcr = 100e3;
  double m_Xt = 0.01;
  double m_sampling = 0.1;
  double m_recovery = 20.;

  // ADC parameters
  unsigned int m_bits = 16;
  double m_range = std::pow(2., static_cast<double>(m_bits)-1.);
  double m_gain = 20.; // in dB i.e. gain(linear) = 10^(m_gain/20)

  // integration parameters
  double m_gateStart = 10.;
  double m_gateEnd = 250.;
  int m_thres = static_cast<int>( 0.5 * (std::pow(10.,m_gain/20.) * std::pow(2., static_cast<double>(m_bits)) / m_range ) );

  bool isFirstEvent = true;
  int verbose = 0;
};

} // namespace drc

#endif
