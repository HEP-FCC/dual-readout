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
  const edm4hep::RawCalorimeterHitCollection* inputRaws = m_rawHits.get();
  const edm4hep::SimCalorimeterHitCollection* inputSims = m_simHits.get();

  // output
  edm4hep::CalorimeterHitCollection* scintHit = m_sHits.createAndPut();
  edm4hep::CalorimeterHitCollection* cherenHit = m_cHits.createAndPut();
  edm4hep::DRrecoCalorimeterHitCollection* DRscintHit = m_DRScintHits.createAndPut();
  edm4hep::DRrecoCalorimeterHitCollection* DRcherenHit = m_DRCherenHits.createAndPut();
  edm4hep::DRrecoCaloAssociationCollection* scintAssocs = m_scintAssocs.createAndPut();
  edm4hep::DRrecoCaloAssociationCollection* cherenAssocs = m_cherenAssocs.createAndPut();

  for (unsigned int idx = 0; idx < inputs->size(); idx++) {
    auto input = inputs->at(idx);
    auto& rawhit = input.getEdm4hepHit();

    auto cID = static_cast<dd4hep::DDSegmentation::CellID>( rawhit.getCellID() );
    int numEta = pSeg->numEta(cID);

    pParamBase = pSeg->setParamBase(numEta);
    int absNumEta = pParamBase->unsignedTowerNo(numEta);

    if (pSeg->IsCerenkov(cID)) {
      auto hit = cherenHit->create();
      auto drHit = DRcherenHit->create();
      auto assoc = cherenAssocs->create();
      assoc.setRec(drHit);
      assoc.setSim(input);

      add(drHit,hit,input,m_calibs.at(absNumEta).first);
    } else {
      auto hit = scintHit->create();
      auto drHit = DRscintHit->create();
      auto assoc = scintAssocs->create();
      assoc.setRec(drHit);
      assoc.setSim(input);

      add(drHit,hit,input,m_calibs.at(absNumEta).second);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode RecoFiber::finalize() {
  m_calibs.clear();

  return GaudiAlgorithm::finalize();
}

void RecoFiber::add(edm4hep::DRrecoCalorimeterHit& drHit, edm4hep::CalorimeterHit& hit, const edm4hep::DRSimCalorimeterHit& input, float calib) {
  auto& rawhit = input.getEdm4hepHit();
  auto cID = static_cast<dd4hep::DDSegmentation::CellID>(rawhit.getCellID());

  hit.setEnergy( static_cast<float>(rawhit.getAmplitude())/calib );
  hit.setPosition( getPosition(cID) );
  hit.setCellID( rawhit.getCellID() );
  addToTimeStruct(drHit,input,calib);

  drHit.setEdm4hepHit(hit);
}

void RecoFiber::addToTimeStruct(edm4hep::DRrecoCalorimeterHit& drHit, const edm4hep::DRSimCalorimeterHit& input, float calib) {
  auto cID = static_cast<dd4hep::DDSegmentation::CellID>(input.getEdm4hepHit().getCellID());
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

    float timeBin = (input.getTimeBegin(bin)+input.getTimeEnd(bin))*dd4hep::nanosecond/2.;

    float numerator = timeBin - std::sqrt(sipmPos.Mag2())/dd4hep::c_light;
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
