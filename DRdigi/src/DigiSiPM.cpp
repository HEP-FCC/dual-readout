#include "DigiSiPM.h"

#include <cmath>
#include <stdexcept>

DECLARE_COMPONENT(DigiSiPM)

DigiSiPM::DigiSiPM(const std::string& aName, ISvcLocator* aSvcLoc) : Gaudi::Algorithm(aName, aSvcLoc) {}

StatusCode DigiSiPM::initialize() {
  StatusCode sc = Gaudi::Algorithm::initialize();

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

StatusCode DigiSiPM::execute(const EventContext&) const {
  const edm4hep::RawTimeSeriesCollection* timeStructs = m_timeStruct.get();
  const edm4hep::RawCalorimeterHitCollection* rawHits = m_rawHits.get();

  edm4hep::TimeSeriesCollection* waveforms = m_waveforms.createAndPut();
  edm4hep::CalorimeterHitCollection* digiHits = m_digiHits.createAndPut();

  for (unsigned int idx = 0; idx < timeStructs->size(); idx++) {
    const auto& timeStruct = timeStructs->at(idx);
    const auto& rawhit = rawHits->at(idx);

    if (timeStruct.getCellID()!=rawhit.getCellID()) {
      error() << "CellIDs of RawCalorimeterHits & RawTimeSeries are different! "
              << "This should never happen." << endmsg;
      return StatusCode::FAILURE;
    }

    std::vector<double> times;
    times.reserve( rawhit.getAmplitude() );

    for (unsigned int bin = 0; bin < timeStruct.adcCounts_size(); bin++) {
      int counts = static_cast<int>( timeStruct.getAdcCounts(bin) );
      double timeBin = timeStruct.getTime() + timeStruct.getInterval()*(static_cast<float>(bin)+0.5);

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
    const double gateEnd = m_gateStart.value() + m_gateL.value();

    digiHit.setEnergy( integral );
    digiHit.setCellID( rawhit.getCellID() );
    // Toa and m_gateStart are in ns
    digiHit.setTime( toa+m_gateStart );
    waveform.setInterval( m_sampling );
    waveform.setTime( timeStruct.getTime() );
    waveform.setCellID( timeStruct.getCellID() );

    // sipm::SiPMAnalogSignal can be iterated as an std::vector<double>
    for (unsigned bin = 0; bin < anaSignal.size(); bin++) {
      double amp = anaSignal[bin];

      double tStart = static_cast<double>(bin)*m_sampling;
      double tEnd = static_cast<double>(bin+1)*m_sampling;
      double center = (tStart+tEnd)/2.;

      if ( center < timeStruct.getTime() )
        continue;

      if ( center > gateEnd )
        continue;

      waveform.addToAmplitude( amp );
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode DigiSiPM::finalize() {
  return Gaudi::Algorithm::finalize();
}
