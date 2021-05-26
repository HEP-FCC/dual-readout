#include "SimG4SaveDRcaloHits.h"

#include "GridDRcalo.h"

// Geant4
#include "G4Event.hh"

// DD4hep
#include "DDG4/Geant4Hits.h"

#include "G4SystemOfUnits.hh"
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

  auto& DRsimCaloHits = pStore->create<edm4hep::DRSimCalorimeterHitCollection>("DRSimCalorimeterHits");
  mDRsimCaloHits = &DRsimCaloHits;
  pWriter->registerForWrite("DRSimCalorimeterHits");

  return;
}

void SimG4SaveDRcaloHits::saveOutput(const G4Event* aEvent) const {
  G4HCofThisEvent* collections = aEvent->GetHCofThisEvent();
  G4VHitsCollection* collect;
  ddDRcalo::DRcaloSiPMHit* hit;

  if (collections != nullptr) {
    for (int iter_coll = 0; iter_coll < collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);

      if (std::find(m_readoutNames.begin(), m_readoutNames.end(), collect->GetName()) != m_readoutNames.end()) {
        size_t n_hit = collect->GetSize();

        for (size_t iter_hit = 0; iter_hit < n_hit; iter_hit++) {
          hit = dynamic_cast<ddDRcalo::DRcaloSiPMHit*>(collect->GetHit(iter_hit));

          auto caloHit = mRawCaloHits->create();
          caloHit.setCellID( static_cast<unsigned long long>(hit->GetSiPMnum()) );
          caloHit.setAmplitude( hit->GetPhotonCount() );

          auto DRcaloHit = mDRsimCaloHits->create();

          for (auto& timeStruct : hit->GetTimeStruct()) {
            DRcaloHit.addToTimeStruct(timeStruct.second);
            DRcaloHit.addToTimeBegin(timeStruct.first.first);
            DRcaloHit.addToTimeEnd(timeStruct.first.second);
          }

          for (auto& wavlen : hit->GetWavlenSpectrum()) {
            DRcaloHit.addToWavlenSpectrum(wavlen.second);
            DRcaloHit.addToWavlenBegin(wavlen.first.first);
            DRcaloHit.addToWavlenEnd(wavlen.first.second);
          }

          DRcaloHit.setEdm4hepHit( caloHit );
        }
      }
    }
  }

  return;
}
