#include "SimG4SaveDRcaloHits.h"

// Geant4
#include "G4Event.hh"

// DD4hep
#include "DD4hep/Detector.h"

DECLARE_COMPONENT(SimG4SaveDRcaloHits)

SimG4SaveDRcaloHits::SimG4SaveDRcaloHits(const std::string& aType, const std::string& aName, const IInterface* aParent)
: GaudiTool(aType, aName, aParent), m_geoSvc("GeoSvc", aName) {
  declareInterface<ISimG4SaveOutputTool>(this);
}

SimG4SaveDRcaloHits::~SimG4SaveDRcaloHits() {}

StatusCode SimG4SaveDRcaloHits::initialize() {
  if (GaudiTool::initialize().isFailure())
    return StatusCode::FAILURE;

  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }

  auto lcdd = m_geoSvc->lcdd();
  auto allReadouts = lcdd->readouts();
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

StatusCode SimG4SaveDRcaloHits::finalize() { return GaudiTool::finalize(); }

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

          unsigned nbinTime = static_cast<unsigned>(std::floor((timeEnd-timeStart)/samplingT));
          unsigned nbinWav = static_cast<unsigned>(std::floor((wavMax-wavMin)/samplingW));
          float peakTime = 0.;
          int peakVal = 0;

          for (unsigned itime = 0; itime < nbinTime; itime++) {
            float cen = timeStart + static_cast<float>(itime)*samplingT/2.;
            int count = 0;

            if ( timemap.find(cen)!=timemap.end() )
              count = timemap.at(cen);

            int candidate = std::max( peakVal, count );

            if ( peakVal < candidate ) {
              peakVal = candidate;
              peakTime = cen;
            }

            timeStruct.addToAdcCounts(count);
          }

          for (unsigned iwav = 0; iwav < nbinWav; iwav++) {
            float cen = wavMin + static_cast<float>(iwav)*samplingW/2.;
            int count = 0;

            if ( wavmap.find(cen)!=wavmap.end() )
              count = wavmap.at(cen);

            wavStruct.addToAdcCounts(count);
          }

          caloHit.setCellID( cellID );
          caloHit.setAmplitude( hit->GetPhotonCount() );
          caloHit.setTimeStamp( static_cast<int>( peakTime / samplingT ) );
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}
