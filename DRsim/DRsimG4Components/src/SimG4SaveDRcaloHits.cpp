#include "SimG4SaveDRcaloHits.h"

// Geant4
#include "G4Event.hh"

// DD4hep
#include "DD4hep/Detector.h"

DECLARE_COMPONENT(SimG4SaveDRcaloHits)

SimG4SaveDRcaloHits::SimG4SaveDRcaloHits(const std::string& aType, const std::string& aName, const IInterface* aParent)
: AlgTool(aType, aName, aParent), m_geoSvc("GeoSvc", aName) {
  declareInterface<ISimG4SaveOutputTool>(this);
}

SimG4SaveDRcaloHits::~SimG4SaveDRcaloHits() {}

StatusCode SimG4SaveDRcaloHits::initialize() {
  if (AlgTool::initialize().isFailure())
    return StatusCode::FAILURE;

  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }

  auto det = m_geoSvc->getDetector();
  auto allReadouts = det->readouts();
  for (auto& readoutName : m_readoutNames) {
    if (allReadouts.find(readoutName) == allReadouts.end()) {
      error() << "Readout " << readoutName << " not found! Please check tool configuration." << endmsg;
      return StatusCode::FAILURE;
    } else {
      debug() << "Hits will be saved to EDM from the collection " << readoutName << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode SimG4SaveDRcaloHits::finalize() { return AlgTool::finalize(); }

StatusCode SimG4SaveDRcaloHits::saveOutput(const G4Event& aEvent) {
  G4HCofThisEvent* collections = aEvent.GetHCofThisEvent();
  G4VHitsCollection* collect;
  drc::DRcaloSiPMHit* hit;

  if (collections != nullptr) {
    edm4hep::RawCalorimeterHitCollection* caloHits = mRawCaloHits.createAndPut();
    edm4hep::RawTimeSeriesCollection* timeStructs = mTimeStruct.createAndPut();
    edm4hep::RawTimeSeriesCollection* wavStructs = mWavlenStruct.createAndPut();

    for (int iter_coll = 0; iter_coll < collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);

      if (std::find(m_readoutNames.begin(), m_readoutNames.end(), collect->GetName()) != m_readoutNames.end()) {
        size_t n_hit = collect->GetSize();

        for (size_t iter_hit = 0; iter_hit < n_hit; iter_hit++) {
          hit = dynamic_cast<drc::DRcaloSiPMHit*>(collect->GetHit(iter_hit));

          uint64_t cellID = static_cast<unsigned long long>(hit->GetSiPMnum());

          auto caloHit = caloHits->create();
          auto timeStruct = timeStructs->create();
          auto wavStruct = wavStructs->create();

          float samplingT = hit->GetSamplingTime();
          float timeStart = hit->GetTimeStart();
          float timeEnd = hit->GetTimeEnd();
          auto& timemap = hit->GetTimeStruct();
          timeStruct.setInterval(samplingT);
          timeStruct.setTime(timeStart);
          timeStruct.setCharge( static_cast<float>(hit->GetPhotonCount()) );
          timeStruct.setCellID( cellID );

          // abuse time series for saving wavelength spectrum (for R&D purpose)
          float samplingW = hit->GetSamplingWavlen();
          float wavMax = hit->GetWavlenMax();
          float wavMin = hit->GetWavlenMin();
          auto& wavmap = hit->GetWavlenSpectrum();
          wavStruct.setInterval(samplingW);
          wavStruct.setTime(wavMin);
          wavStruct.setCharge( static_cast<float>(hit->GetPhotonCount()) );
          wavStruct.setCellID( cellID );

          unsigned nbinTime = static_cast<unsigned>((timeEnd-timeStart)/samplingT);
          unsigned nbinWav = static_cast<unsigned>((wavMax-wavMin)/samplingW);
          int peakTime = 0.;
          int peakVal = 0;

          // same as the ROOT TH1 binning scheme (0: underflow, nbin+1:overflow)
          for (unsigned itime = 1; itime < nbinTime+1; itime++) {
            int count = 0;

            if ( timemap.find(itime)!=timemap.end() )
              count = timemap.at(itime);

            int candidate = std::max( peakVal, count );

            if ( peakVal < candidate ) {
              peakVal = candidate;
              peakTime = itime;
            }

            timeStruct.addToAdcCounts(count);
          }

          for (unsigned iwav = 1; iwav < nbinWav+1; iwav++) {
            int count = 0;

            if ( wavmap.find(iwav)!=wavmap.end() )
              count = wavmap.at(iwav);

            wavStruct.addToAdcCounts(count);
          }

          caloHit.setCellID( cellID );
          caloHit.setAmplitude( hit->GetPhotonCount() );
          caloHit.setTimeStamp( peakTime-1 + static_cast<int>(timeStart/samplingT) );
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}
