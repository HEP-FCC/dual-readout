#include "DigiSiPM.h"

#include <cmath>
#include <stdexcept>

DECLARE_COMPONENT(DigiSiPM)

DigiSiPM::DigiSiPM(const std::string& aName, ISvcLocator* aSvcLoc) : GaudiAlgorithm(aName, aSvcLoc) {}

StatusCode DigiSiPM::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();

  if (sc.isFailure()) return sc;

  sipm::SiPMProperties properties;
  properties.setSignalLength(m_sigLength);
  properties.setSize(m_sipmSize);
  properties.setDcr(m_Dcr);
  properties.setXt(m_Xt);
  properties.setSampling(m_sampling);
  properties.setRecoveryTime(m_recovery);
  properties.setPitch(m_cellPitch);
  properties.setAp(m_afterpulse);
  properties.setFallTimeFast(m_falltimeFast);
  properties.setRiseTime(m_risetime);
  properties.setSnr(m_snr);

  m_sensor = std::make_unique<sipm::SiPMSensor>(properties); // must be constructed from SiPMProperties

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

    // Using only analog signal (ADC conversion is still experimental)
    const sipm::SiPMAnalogSignal anaSignal = m_sensor->signal();

    const double integral = anaSignal.integral(m_gateStart,m_gateL,m_thres); // (intStart, intGate, threshold)
    const double toa = anaSignal.toa(m_gateStart,m_gateL,m_thres);           // (intStart, intGate, threshold)

    digiHit.setAmplitude( integral );
    digiHit.setCellID( rawhit.getCellID() );
    // Toa and m_gateStart are in ns
    digiHit.setTimeStamp( static_cast<int>((toa+m_gateStart)/m_sampling) );
    waveform.setAssocObj( edm4hep::ObjectID( digiHit.getObjectID() ) );
    waveform.setSampling( m_sampling );

    // sipm::SiPMAnalogSignal can be iterated as an std::vector<double>
    for (unsigned bin = 0; bin < anaSignal.size(); bin++) {
      double amp = anaSignal[bin];

      if (amp < m_thres) continue;

      double tStart = static_cast<double>(bin)*m_sampling;
      double tEnd = static_cast<double>(bin+1)*m_sampling;

      waveform.addToContents( amp );
      waveform.addToCenters( (tStart+tEnd)/2. );
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode DigiSiPM::finalize() {
  return GaudiAlgorithm::finalize();
}
