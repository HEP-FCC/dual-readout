#include "DigiPodio.h"

#include <cmath>
#include <stdexcept>

drc::DigiPodio::DigiPodio(const std::string filenameIn, const std::string filenameOut) {
  m_filenameIn = filenameIn;
  m_filenameOut = filenameOut;
}

void drc::DigiPodio::initialize() {
  m_reader = std::make_unique<podio::ROOTReader>();
  m_reader->openFile(m_filenameIn);

  m_store = std::make_unique<podio::EventStore>();
  m_store->setReader(m_reader.get());

  m_writer = std::make_unique<podio::ROOTWriter>(m_filenameOut, m_store.get());

  m_writer->registerForWrite("SimCalorimeterHits");
  m_writer->registerForWrite("RawCalorimeterHits");
  m_writer->registerForWrite("DRSimCalorimeterHits");
  m_writer->registerForWrite("EventHeader");

  registerForWrite<edm4hep::RawCalorimeterHitCollection>(m_digiHits,"DigiCalorimeterHits");
  registerForWrite<edm4hep::DRSimCalorimeterHitCollection>(m_DRdigiHits,"DRdigiCalorimeterHits");

  m_sensor = std::make_unique<sipm::SiPMSensor>();
  m_sensor->properties().setDcr(m_Dcr);
  m_sensor->properties().setXt(m_Xt);
  m_sensor->properties().setSampling(m_sampling);
  m_sensor->properties().setRecoveryTime(m_recovery);

  m_adc = std::make_unique<sipm::SiPMAdc>(m_bits,m_range,m_gain);

  std::cout << "DigiPodio initialized" << std::endl;

  return;
}

void drc::DigiPodio::execute() {
  auto& inputs_ = m_store->get<edm4hep::DRSimCalorimeterHitCollection>("DRSimCalorimeterHits");
  const edm4hep::DRSimCalorimeterHitCollection* inputs = &inputs_;

  if (!isFirstEvent) {
    create<edm4hep::RawCalorimeterHitCollection>(m_digiHits,"DigiCalorimeterHits");
    create<edm4hep::DRSimCalorimeterHitCollection>(m_DRdigiHits,"DRdigiCalorimeterHits");
  } else {
    isFirstEvent = false;
  }

  for (unsigned int idx = 0; idx < inputs->size(); idx++) {
    auto input = inputs->at(idx);
    auto& rawhit = input.getEdm4hepHit();

    std::vector<double> times;
    times.reserve( rawhit.getAmplitude() );

    for (unsigned int bin = 0; bin < input.timeStruct_size(); bin++) {
      int counts = input.getTimeStruct(bin);
      double timeBin = (input.getTimeBegin(bin)+input.getTimeEnd(bin))/2.;

      for (int num = 0; num < counts; num++)
        times.emplace_back(timeBin);
    }

    m_sensor->resetState();
    m_sensor->addPhotons(times); // Sets photon times (times are in ns) (not appending)
    m_sensor->runEvent();        // Runs the simulation

    auto digiHit = m_digiHits->create();
    auto DRdigiHit = m_DRdigiHits->create();

    sipm::SiPMAnalogSignal anaSignal = m_sensor->signal();
    sipm::SiPMDigitalSignal digiSignal = m_adc->digitize(anaSignal);

    int integral = digiSignal.integral(m_gateStart,m_gateEnd,m_thres);   // (intStart, intGate, threshold)
    double toa = digiSignal.toa(m_gateStart,m_gateEnd,m_thres);          // (intStart, intGate, threshold)

    digiHit.setAmplitude( integral );
    digiHit.setCellID( rawhit.getCellID() );
    digiHit.setTimeStamp( toa );
    DRdigiHit.setEdm4hepHit(digiHit);

    std::vector<int> waveform = digiSignal.waveform();

    for (unsigned bin = 0; bin < waveform.size(); bin++) {
      int amp = waveform.at(bin);

      if (amp < m_thres) continue;

      float tStart = static_cast<float>(bin)*m_sampling;
      float tEnd = static_cast<float>(bin+1)*m_sampling;

      DRdigiHit.addToTimeStruct( amp );
      DRdigiHit.addToTimeBegin( tStart );
      DRdigiHit.addToTimeEnd( tEnd );
    }
  }

  return;
}

void drc::DigiPodio::finalize() {
  m_reader->closeFile();
  m_writer->finish();

  return;
}

template <typename T>
void drc::DigiPodio::create(T*& pCollection, std::string name) {
  auto& collection = m_store->create<T>(name);
  pCollection = &collection;
}

template <typename T>
void drc::DigiPodio::registerForWrite(T*& pCollection, std::string name) {
  create(pCollection, name);
  m_writer->registerForWrite(name);
}
