#include "RecoFiber.h"

#include "GridDRcalo.h"

#include "DD4hep/DD4hepUnits.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "k4FWCore/PodioDataSvc.h"
#include "TFile.h"

#include <cmath>

DECLARE_COMPONENT(RecoFiber)

RecoFiber::RecoFiber(const std::string& aName, ISvcLocator* aSvcLoc) : GaudiAlgorithm(aName, aSvcLoc), m_geoSvc("GeoSvc", aName), m_dataSvc("EventDataSvc", aName) {
  declareProperty("GeoSvc", m_geoSvc);
  declareProperty("EventDataSvc", m_dataSvc);
  declareProperty("DRSimCalorimeterHits", m_DRsimHits, "DRsim hit collection (input)");
  declareProperty("RawCalorimeterHits", m_rawHits, "Raw hit collection (input)");

  declareProperty("ScintillationHits", m_sHits, "Scintillation hit collection (output)");
  declareProperty("CherenkovHits", m_cHits, "Cherenkov hit collection (output)");
  declareProperty("DRrecoScintillationHits", m_DRScintHits, "Extended scintillation hit collection (output)");
  declareProperty("DRrecoCherenkovHits", m_DRCherenHits, "Extended Cherenkov hit collection (output)");

  pSeg = nullptr;
  pParamBase = nullptr;
}

StatusCode RecoFiber::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();

  if (sc.isFailure()) return sc;

  if (!m_geoSvc) {
    error() << "Unable to locate Geometry service." << endmsg;
    return StatusCode::FAILURE;
  }

  if (!m_dataSvc) {
    error() << "Unable to locate Event data service." << endmsg;
    return StatusCode::FAILURE;
  }

  pSeg = dynamic_cast<dd4hep::DDSegmentation::GridDRcalo*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());

  readCSV(m_calibPath);

  if ( static_cast<int>(m_calibs.size()) != pSeg->paramBarrel()->GetTotTowerNum() + pSeg->paramEndcap()->GetTotTowerNum() ) {
    error() << "Number of calibration constants does not match with the number of towers" << endmsg;
    return StatusCode::FAILURE;
  }

  PodioDataSvc* pds = dynamic_cast<PodioDataSvc*>( m_dataSvc.get() );

  if (pds!=nullptr) {
    // FIXME read timing bins from podio metadata
  } else {
    error() << "Failed to retrieve collection metadata" << endmsg;
    return StatusCode::FAILURE;
  }

  info() << "RecoFiber initialized" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode RecoFiber::execute() {
  // input
  const edm4hep::DRSimCalorimeterHitCollection* inputs = m_DRsimHits.get();

  std::vector<float> timeBinLow {};
  std::vector<float> timeBinCenter {};

  // #FIXME read timing bins from podio metadata
  timeBinLow.push_back(0.);
  float timeStart = 10.;

  for (int bin = 0; bin < 600; bin++)
    timeBinLow.push_back(timeStart + static_cast<float>(bin)*0.1);

  for (unsigned int bin = 0; bin < timeBinLow.size()-1; bin++)
    timeBinCenter.push_back( ( timeBinLow.at(bin)+timeBinLow.at(bin+1) )/2. );
  timeBinCenter.push_back(99999.); // # FIXME hardcoded overflow bin

  // output
  edm4hep::CalorimeterHitCollection* scintHit = m_sHits.createAndPut();
  edm4hep::CalorimeterHitCollection* cherenHit = m_cHits.createAndPut();
  edm4hep::DRrecoCalorimeterHitCollection* DRscintHit = m_DRScintHits.createAndPut();
  edm4hep::DRrecoCalorimeterHitCollection* DRcherenHit = m_DRCherenHits.createAndPut();

  for (auto& input : *inputs) {
    auto& rawhit = input.getEdm4hepRawCalorimeterHit();

    auto cID = static_cast<dd4hep::DDSegmentation::CellID>( rawhit.getCellID() );
    int numEta = pSeg->numEta(cID);

    pParamBase = pSeg->setParamBase(numEta);
    int absNumEta = pParamBase->unsignedTowerNo(numEta);

    if (pSeg->IsCerenkov(cID)) {
      auto hit = cherenHit->create();
      auto drHit = DRcherenHit->create();

      add(drHit,hit,input,timeBinCenter,m_calibs.at(absNumEta).first);
    } else {
      auto hit = scintHit->create();
      auto drHit = DRscintHit->create();

      add(drHit,hit,input,timeBinCenter,m_calibs.at(absNumEta).second);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode RecoFiber::finalize() {
  m_calibs.clear();

  return GaudiAlgorithm::finalize();
}

void RecoFiber::add(edm4hep::DRrecoCalorimeterHit& drHit, edm4hep::CalorimeterHit& hit, const edm4hep::DRSimCalorimeterHit& input,
                    const std::vector<float>& timeBinCenter, float calib) {
  auto& rawhit = input.getEdm4hepRawCalorimeterHit();
  auto cID = static_cast<dd4hep::DDSegmentation::CellID>(rawhit.getCellID());

  hit.setEnergy( static_cast<float>(rawhit.getAmplitude())/calib );
  hit.setPosition( getPosition(cID) );
  addToTimeStruct(drHit,input,timeBinCenter,calib);

  drHit.setEdm4hepDRSimCalorimeterHit(input);
  drHit.setEdm4hepCalorimeterHit(hit);
}

void RecoFiber::addToTimeStruct(edm4hep::DRrecoCalorimeterHit& drHit, const edm4hep::DRSimCalorimeterHit& input,
                                const std::vector<float>& timeBinCenter, float calib) {
  auto cID = static_cast<dd4hep::DDSegmentation::CellID>(input.getEdm4hepRawCalorimeterHit().getCellID());
  int numPhi = pSeg->numPhi( cID );
  auto towerPos = pParamBase->GetTowerPos(numPhi);
  auto waferPos = pParamBase->GetSipmLayerPos(numPhi);
  auto sipmPos_ = pSeg->position(cID);
  dd4hep::Position sipmPos(sipmPos_.x(),sipmPos_.y(),sipmPos_.z()); // type cast from dd4hep::DDSegmentation::Vector3D to dd4heo::Position

  auto fiberDir = waferPos - towerPos; // outward direction
  auto fiberUnit = fiberDir.Unit();

  float effVelocity = pSeg->IsCerenkov(cID) ? m_cherenSpeed*dd4hep::millimeter/dd4hep::nanosecond : m_scintSpeed*dd4hep::millimeter/dd4hep::nanosecond;
  float invVminusInvC = 1./effVelocity - 1./dd4hep::c_light;

  for (unsigned int bin = 0; bin < input.timeStruct_size(); bin++) {
    float edep = static_cast<float>( input.getTimeStruct(bin) )/calib;

    float numerator = timeBinCenter.at(bin)*dd4hep::nanosecond/CLHEP::nanosecond - std::sqrt(sipmPos.Mag2())/dd4hep::c_light;
    auto pos = sipmPos - ( numerator/invVminusInvC )*fiberUnit;
    edm4hep::Vector3f posEdm(pos.x() * CLHEP::millimeter/dd4hep::millimeter,
                             pos.y() * CLHEP::millimeter/dd4hep::millimeter,
                             pos.z() * CLHEP::millimeter/dd4hep::millimeter);

    drHit.addToTimeStruct(edep);
    drHit.addToPosition( posEdm );
  }
}

edm4hep::Vector3f RecoFiber::getPosition(dd4hep::DDSegmentation::CellID& cID) {
  auto globalPos = pSeg->position( cID );
  return { static_cast<float>( globalPos.x() * CLHEP::millimeter/dd4hep::millimeter ),
           static_cast<float>( globalPos.y() * CLHEP::millimeter/dd4hep::millimeter ),
           static_cast<float>( globalPos.z() * CLHEP::millimeter/dd4hep::millimeter ) };
}

void RecoFiber::readCSV(std::string filename) {
  std::ifstream in;
  int i;
  float ceren, scint;

  in.open(filename,std::ios::in);
  while (true) {
    in >> i >> ceren >> scint;
    if (!in.good()) break;
    m_calibs.push_back(std::make_pair(ceren,scint));
  }
  in.close();
}
