#include "DRcalib2D.h"

#include "GridDRcalo.h"

#include "DD4hep/DD4hepUnits.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

DECLARE_COMPONENT(DRcalib2D)

DRcalib2D::DRcalib2D(const std::string& aName, ISvcLocator* aSvcLoc) : GaudiAlgorithm(aName, aSvcLoc), m_geoSvc("GeoSvc", aName) {
  declareProperty("GeoSvc", m_geoSvc);

  pSeg = nullptr;
  pParamBase = nullptr;
}

StatusCode DRcalib2D::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();

  if (sc.isFailure()) return sc;

  if (!m_geoSvc) {
    error() << "Unable to locate Geometry service." << endmsg;
    return StatusCode::FAILURE;
  }

  pSeg = dynamic_cast<dd4hep::DDSegmentation::GridDRcalo*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());

  readCSV(m_calibPath);

  if ( static_cast<int>(m_calibs.size()) != pSeg->paramBarrel()->GetTotTowerNum() + pSeg->paramEndcap()->GetTotTowerNum() ) {
    error() << "Number of calibration constants does not match with the number of towers" << endmsg;
    return StatusCode::FAILURE;
  }

  info() << "DRcalib2D initialized" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode DRcalib2D::execute() {
  const edm4hep::RawCalorimeterHitCollection* digiHits = m_digiHits.get();
  edm4hep::CalorimeterHitCollection* caloHits = m_caloHits.createAndPut();

  for (unsigned int idx = 0; idx < digiHits->size(); idx++) {
    const auto& digiHit = digiHits->at(idx);

    auto cID = static_cast<dd4hep::DDSegmentation::CellID>( digiHit.getCellID() );
    int numEta = pSeg->numEta(cID);

    pParamBase = pSeg->setParamBase(numEta);
    int absNumEta = pParamBase->unsignedTowerNo(numEta);

    auto caloHit = caloHits->create();
    caloHit.setPosition( getPosition(cID) );
    caloHit.setCellID( digiHit.getCellID() );
    caloHit.setTime( static_cast<double>(digiHit.getTimeStamp())*m_sampling );

    bool isCeren = pSeg->IsCerenkov(cID);
    float calib = isCeren ? m_calibs.at(absNumEta).first : m_calibs.at(absNumEta).second;
    caloHit.setType( static_cast<int>( isCeren ) );
    caloHit.setEnergy( static_cast<float>(digiHit.getAmplitude())/calib );
  }

  return StatusCode::SUCCESS;
}

StatusCode DRcalib2D::finalize() {
  m_calibs.clear();

  return GaudiAlgorithm::finalize();
}

edm4hep::Vector3f DRcalib2D::getPosition(dd4hep::DDSegmentation::CellID& cID) {
  auto globalPos = pSeg->position( cID );
  return { static_cast<float>( globalPos.x() * CLHEP::millimeter/dd4hep::millimeter ),
           static_cast<float>( globalPos.y() * CLHEP::millimeter/dd4hep::millimeter ),
           static_cast<float>( globalPos.z() * CLHEP::millimeter/dd4hep::millimeter ) };
}

void DRcalib2D::readCSV(std::string filename) {
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
