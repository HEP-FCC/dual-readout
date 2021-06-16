#include "SimG4SaveDRcaloHits.h"

#include "GridDRcalo.h"

// Geant4
#include "G4Event.hh"

// DD4hep
#include "DDG4/Geant4Hits.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "DD4hep/DD4hepUnits.h"

#include <stdexcept>
#include <functional>

SimG4SaveDRcaloHits::SimG4SaveDRcaloHits(podio::EventStore* store, podio::ROOTWriter* writer)
: pStore(store), pWriter(writer) {
  m_geoSvc = GeoSvc::GetInstance();
  m_readoutNames = {"DRcaloSiPMreadout"};

  initialize();

  if (m_geoSvc==0) throw std::runtime_error("Attempt to save hits while GeoSvc is not initialized!");
}

SimG4SaveDRcaloHits::~SimG4SaveDRcaloHits() {}

void SimG4SaveDRcaloHits::initialize() {
  // DD4hep readouts
  auto lcdd = m_geoSvc->lcdd();
  auto allReadouts = lcdd->readouts();
  for (auto& readoutName : m_readoutNames) {
    if (allReadouts.find(readoutName) == allReadouts.end()) {
      throw std::runtime_error("Readout " + readoutName + " not found! Please check tool configuration.");
    } else {
      std::cout << "Hits will be saved to EDM from the collection " << readoutName << std::endl;
    }
  }

  auto& rawCaloHits = pStore->create<edm4hep::RawCalorimeterHitCollection>("RawCalorimeterHits");
  mRawCaloHits = &rawCaloHits;
  pWriter->registerForWrite("RawCalorimeterHits");

  auto& timeStruct = pStore->create<edm4hep::SparseVectorCollection>("RawTimeStructs");
  mTimeStruct = &timeStruct;
  pWriter->registerForWrite("RawTimeStructs");

  auto& wavlenStruct = pStore->create<edm4hep::SparseVectorCollection>("RawWavlenStructs");
  mWavlenStruct = &wavlenStruct;
  pWriter->registerForWrite("RawWavlenStructs");

  return;
}

void SimG4SaveDRcaloHits::saveOutput(const G4Event* aEvent) const {
  G4HCofThisEvent* collections = aEvent->GetHCofThisEvent();
  G4VHitsCollection* collect;
  drc::DRcaloSiPMHit* hit;

  if (collections != nullptr) {
    for (int iter_coll = 0; iter_coll < collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);

      if (std::find(m_readoutNames.begin(), m_readoutNames.end(), collect->GetName()) != m_readoutNames.end()) {
        size_t n_hit = collect->GetSize();

        for (size_t iter_hit = 0; iter_hit < n_hit; iter_hit++) {
          hit = dynamic_cast<drc::DRcaloSiPMHit*>(collect->GetHit(iter_hit));

          auto caloHit = mRawCaloHits->create();
          auto timeStruct = mTimeStruct->create();
          auto wavStruct = mWavlenStruct->create();

          float sumT = 0.;
          float samplingT = hit->GetSamplingTime();
          for (auto& i_timeStruct : hit->GetTimeStruct()) {
            timeStruct.addToContents(i_timeStruct.second);
            timeStruct.addToCenters( i_timeStruct.first );
            sumT += i_timeStruct.first;
          }

          caloHit.setCellID( static_cast<unsigned long long>(hit->GetSiPMnum()) );
          caloHit.setAmplitude( hit->GetPhotonCount() );
          caloHit.setTimeStamp( static_cast<int>( sumT / static_cast<float>(hit->GetTimeStruct().size()) / samplingT ) );
          timeStruct.setSampling( samplingT );
          timeStruct.setAssocObj( edm4hep::ObjectID( caloHit.getObjectID() ) );

          float sumW = 0.;
          float samplingW = hit->GetSamplingWavlen();
          for (auto& i_wavlen : hit->GetWavlenSpectrum()) {
            wavStruct.addToContents(i_wavlen.second);
            wavStruct.addToCenters( i_wavlen.first );
            sumW += i_wavlen.first;
          }
          wavStruct.setSampling( samplingW );
          wavStruct.setAssocObj( edm4hep::ObjectID( caloHit.getObjectID() ) );
        }
      }
    }
  }

  return;
}
