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
    edm4hep::SparseVectorCollection* timeStructs = mTimeStruct.createAndPut();
    edm4hep::SparseVectorCollection* wavStructs = mWavlenStruct.createAndPut();

    for (int iter_coll = 0; iter_coll < collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);

      if (std::find(m_readoutNames.begin(), m_readoutNames.end(), collect->GetName()) != m_readoutNames.end()) {
        size_t n_hit = collect->GetSize();

        for (size_t iter_hit = 0; iter_hit < n_hit; iter_hit++) {
          hit = dynamic_cast<drc::DRcaloSiPMHit*>(collect->GetHit(iter_hit));

          auto caloHit = caloHits->create();
          auto timeStruct = timeStructs->create();
          auto wavStruct = wavStructs->create();

          float peakTime = 0.;
          int peakVal = 0;
          float samplingT = hit->GetSamplingTime();
          for (auto& i_timeStruct : hit->GetTimeStruct()) {
            timeStruct.addToContents(i_timeStruct.second);
            timeStruct.addToCenters( i_timeStruct.first );

            int candidate = std::max( peakVal, i_timeStruct.second );

            if ( peakVal < candidate ) {
              peakVal = candidate;
              peakTime = i_timeStruct.first;
            }
          }

          caloHit.setCellID( static_cast<unsigned long long>(hit->GetSiPMnum()) );
          caloHit.setAmplitude( hit->GetPhotonCount() );
          caloHit.setTimeStamp( static_cast<int>( peakTime / samplingT ) );
          timeStruct.setSampling( samplingT );
          timeStruct.setAssocObj( edm4hep::ObjectID( caloHit.getObjectID() ) );

          float samplingW = hit->GetSamplingWavlen();
          for (auto& i_wavlen : hit->GetWavlenSpectrum()) {
            wavStruct.addToContents(i_wavlen.second);
            wavStruct.addToCenters( i_wavlen.first );
          }
          wavStruct.setSampling( samplingW );
          wavStruct.setAssocObj( edm4hep::ObjectID( caloHit.getObjectID() ) );
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}
