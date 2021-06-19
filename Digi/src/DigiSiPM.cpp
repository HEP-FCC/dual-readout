#include "DigiSiPM.h"

#include <cmath>
#include <stdexcept>

DECLARE_COMPONENT(DigiSiPM)

DigiSiPM::DigiSiPM(const std::string& aName, ISvcLocator* aSvcLoc) : GaudiAlgorithm(aName, aSvcLoc) {}

StatusCode DigiSiPM::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();

  if (sc.isFailure()) return sc;

  m_sensor = std::make_unique<sipm::SiPMSensor>();
  m_sensor->properties().setDcr(m_Dcr);
  m_sensor->properties().setXt(m_Xt);
  m_sensor->properties().setSampling(m_sampling);
  m_sensor->properties().setRecoveryTime(m_recovery);

  m_adc = std::make_unique<sipm::SiPMAdc>(m_bits,m_range,m_gain);

  info() << "DigiSiPM initialized" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode DigiSiPM::execute() {
  const edm4hep::SparseVectorCollection* timeStructs = m_timeStruct.get();
  const edm4hep::RawCalorimeterHitCollection* rawHits = m_rawHits.get();

  edm4hep::SparseVectorCollection* waveforms = m_waveforms.createAndPut();
  edm4hep::RawCalorimeterHitCollection* digiHits = m_digiHits.createAndPut();

  for (unsigned int idx = 0; idx < timeStructs->size(); idx++) {
    const auto& timeStruct = timeStructs->at(idx);
    const auto& rawhit = rawHits->at(timeStruct.getAssocObj().index);

    std::vector<double> times;
    times.reserve( rawhit.getAmplitude() );

    for (unsigned int bin = 0; bin < timeStruct.contents_size(); bin++) {
      int counts = static_cast<int>( timeStruct.getContents(bin) );
      double timeBin = timeStruct.getCenters(bin);

      for (int num = 0; num < counts; num++)
        times.emplace_back(timeBin);
    }

    m_sensor->resetState();
    m_sensor->addPhotons(times); // Sets photon times (times are in ns) (not appending)
    m_sensor->runEvent();        // Runs the simulation

    auto digiHit = digiHits->create();
    auto waveform = waveforms->create();

    sipm::SiPMAnalogSignal anaSignal = m_sensor->signal();
    sipm::SiPMDigitalSignal digiSignal = m_adc->digitize(anaSignal);

    int integral = digiSignal.integral(m_gateStart,m_gateEnd,m_thres);   // (intStart, intGate, threshold)
    double toa = digiSignal.toa(m_gateStart,m_gateEnd,m_thres);          // (intStart, intGate, threshold)

    digiHit.setAmplitude( integral );
    digiHit.setCellID( rawhit.getCellID() );
    digiHit.setTimeStamp( static_cast<int>(toa/m_sampling) );
    waveform.setAssocObj( edm4hep::ObjectID( digiHit.getObjectID() ) );
    waveform.setSampling( m_sampling );

    std::vector<int> waveformVec = digiSignal.waveform();

    for (unsigned bin = 0; bin < waveformVec.size(); bin++) {
      int amp = waveformVec.at(bin);

      if (amp < m_thres) continue;

      float tStart = static_cast<float>(bin)*m_sampling;
      float tEnd = static_cast<float>(bin+1)*m_sampling;

      waveform.addToContents( amp );
      waveform.addToCenters( (tStart+tEnd)/2. );
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode DigiSiPM::finalize() {
  return GaudiAlgorithm::finalize();
}
