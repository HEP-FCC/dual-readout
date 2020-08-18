#include "SimG4SaveDRcaloHits.h"

#include "DRcaloSiPMHit.h"
#include "GridDRcalo.h"

// Geant4
#include "G4Event.hh"

// DD4hep
#include "DDG4/Geant4Hits.h"

#include <stdexcept>

SimG4SaveDRcaloHits::SimG4SaveDRcaloHits() {
  m_geoSvc = GeoSvc::GetInstance();
  m_readoutNames = {"DRcaloSiPMreadout"};

  initialize();

  if (m_geoSvc==0) throw std::runtime_error("Attempt to save hits while GeoSvc is not initialized!");
}

SimG4SaveDRcaloHits::~SimG4SaveDRcaloHits() {}

void SimG4SaveDRcaloHits::initialize() {
  auto lcdd = m_geoSvc->lcdd();
  auto allReadouts = lcdd->readouts();
  for (auto& readoutName : m_readoutNames) {
    if (allReadouts.find(readoutName) == allReadouts.end()) {
      throw std::runtime_error("Readout " + readoutName + " not found! Please check tool configuration.");
    } else {
      std::cout << "Hits will be saved to EDM from the collection " << readoutName << std::endl;
    }
  }
  return;
}

void SimG4SaveDRcaloHits::saveOutput(const G4Event* aEvent) {
  G4HCofThisEvent* collections = aEvent->GetHCofThisEvent();
  G4VHitsCollection* collect;
  ddDRcalo::DRcaloSiPMHit* hit;

  std::map<int, DRsimInterface::DRsimTowerData> towerMap;

  if (collections != nullptr) {
    for (int iter_coll = 0; iter_coll < collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);

      if (std::find(m_readoutNames.begin(), m_readoutNames.end(), collect->GetName()) != m_readoutNames.end()) {
        size_t n_hit = collect->GetSize();

        dd4hep::DDSegmentation::GridDRcalo* segmentation = dynamic_cast<dd4hep::DDSegmentation::GridDRcalo*>(m_geoSvc->lcdd()->readout(collect->GetName()).segmentation().segmentation());

        for (size_t iter_hit = 0; iter_hit < n_hit; iter_hit++) {
          hit = dynamic_cast<ddDRcalo::DRcaloSiPMHit*>(collect->GetHit(iter_hit));

          DRsimInterface::DRsimSiPMData sipmData;
          sipmData.count = hit->GetPhotonCount();
          sipmData.SiPMnum = static_cast<long long int>(hit->GetSiPMnum());
          sipmData.timeStruct = hit->GetTimeStruct();
          sipmData.wavlenSpectrum = hit->GetWavlenSpectrum();

          int first32 = segmentation->getFirst32bits(hit->GetSiPMnum());
          auto towerIter = towerMap.find(first32);

          if ( towerIter==towerMap.end() ) {
            DRsimInterface::DRsimTowerData towerData;
            towerData.iTheta = segmentation->numEta(hit->GetSiPMnum());
            towerData.iPhi = segmentation->numPhi(hit->GetSiPMnum());
            towerData.numx = segmentation->numX(hit->GetSiPMnum());
            towerData.numy = segmentation->numY(hit->GetSiPMnum());
            towerData.SiPMs.push_back(sipmData);

            towerMap.insert(std::make_pair(first32,towerData));
          } else {
            towerIter->second.SiPMs.push_back(sipmData);
          }
        }
      }
    }
  }

  for (const auto& theMap : towerMap) {
    fEventData->towers.push_back(theMap.second);
  }

  return;
}
