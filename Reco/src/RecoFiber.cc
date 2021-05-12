#include "RecoFiber.h"

#include "GridDRcalo.h"

#include "DD4hepUnits.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

DECLARE_COMPONENT(RecoFiber)

RecoFiber::RecoFiber(const std::string& aName, ISvcLocator* aSvcLoc) : GaudiAlgorithm(aName, aSvcLoc) {
  declareProperty("DRSimCalorimeterHits", m_DRsimHits, "DRsim hit collection (input)");
  declareProperty("RawCalorimeterHits", m_rawHits, "Raw hit collection (input)");
  declareProperty("colMD", m_colMDs, "Collection metadata");

  declareProperty("ScintillationHits", m_sHits, "Scintillation hit collection (output)");
  declareProperty("CherenkovHits", m_cHits, "Cherenkov hit collection (output)");
  declareProperty("DRrecoScintillationHits", m_DRScintHits, "Extended scintillation hit collection (output)");
  declareProperty("DRrecoCherenkovHits", m_DRCherenHits, "Extended Cherenkov hit collection (output)");

  pSeg = nullptr;
  pParamBase = nullptr;
}

StatusCode initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();

  if (sc.isFailure()) return sc;

  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry service." << endmsg;
    return StatusCode::FAILURE;
  }

  pSeg = dynamic_cast<dd4hep::DDSegmentation::GridDRcalo*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());

  readCSV();

  if ( m_calibs.size() != pSeg->paramBarrel()->GetTotTowerNum() + pSeg->paramEndcap()->GetTotTowerNum() ) {
    error() << "Number of calibration constants does not match with the number of towers" << endmsg;
    return StatusCode::FAILURE;
  }

  // auto eds = ServiceHandle<IDataProviderSvc>("EventDataSvc", "DataHandle");
  // StatusCode sc = eds.retrieve();
  // PodioDataSvc* pds = dynamic_cast<PodioDataSvc*>( eds.get() ); // worst choice

  info() << "RecoFiber initialized" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode RecoFiber::execute() {
  // input
  const edm4hep::DRSimCalorimeterHitCollection* inputs = m_DRsimHits.get();

  auto& colMD = m_colMDs[ inputs->getID() ];
  std::vector<float> timeBinLow {};
  std::vector<float> timeBinCenter {};
  colMD.getFloatVals(m_timeMDkey,timeBinLow);

  for (int bin = 0; bin < timeBinLow.size()-1; bin++)
    timeBinCenter.push_back( ( timeBinLow.at(bin)+timeBinLow.at(bin+1) )/2. );
  timeBinCenter.push_back(99999.); // # FIXME hardcoded overflow bin

  // output
  edm4hep::CalorimeterHitCollection* scintHit = m_sHits.createAndPut();
  edm4hep::CalorimeterHitCollection* cherenHit = m_cHits.createAndPut();
  edm4hep::DRrecoCalorimeterHitCollection* DRscintHit = m_DRScintHits.createAndPut();
  edm4hep::DRrecoCalorimeterHitCollection* DRcherenHit = m_DRCherenHits.createAndPut();

  for (auto& input : *inputs) {
    edm4hep::RawCalorimeterHit& rawhit = input.getEdm4hepRawCalorimeterHit();

    auto cID = static_cast<dd4hep::DDSegmentation::CellID>( rawhit.getCellID() );
    int numEta = pSeg->numEta(cID);

    pParamBase = pSeg->setParamBase(numEta);
    int absNumEta = pParamBase->unsignedTowerNo();

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
  auto sipmPos = pSeg->position(cID);

  auto fiberDir = waferPos - towerPos; // outward direction
  auto fiberUnit = fiberDir.Unit();

  float effVelocity = pSeg->IsCerenkov(cID) ? m_cherenSpeed*dd4hep::millimeter/dd4hep::nanosecond : m_scintSpeed*dd4hep::millimeter/dd4hep::nanosecond;
  float invVminusInvC = 1./effVelocity - 1./dd4hep::c_light;

  for (unsigned int bin = 0; bin < input.timeStruct_size(); bin++) {
    float edep = static_cast<float>( input.getTimeStruct(bin) )/calib;

    float numerator = timeBinCenter.at(bin)*dd4hep::nanosecond/CLHEP::nanosecond - sipmPos.R()/dd4hep::c_light;
    auto pos = sipmPos - ( numerator/invVminusInvC )*fiberUnit;

    drHit.addToTimeStruct(edep);
    drHit.addToPosition( {pos.x() * CLHEP::millimeter/dd4hep::millimeter,
                          pos.y() * CLHEP::millimeter/dd4hep::millimeter,
                          pos.z() * CLHEP::millimeter/dd4hep::millimeter} );
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
