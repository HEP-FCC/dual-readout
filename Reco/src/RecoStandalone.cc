#include "RecoStandalone.h"

#include "GridDRcalo.h"

#include "DD4hep/DD4hepUnits.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "TFile.h"

#include <cmath>
#include <stdexcept>

RecoStandalone::RecoStandalone(const std::string filenameIn, const std::string filenameOut) {
  m_geoSvc = nullptr;
  pSeg = nullptr;
  pParamBase = nullptr;

  m_filenameIn = filenameIn;
  m_filenameOut = filenameOut;
}

void RecoStandalone::initialize() {
  m_geoSvc = GeoSvc::GetInstance();

  if (m_geoSvc==nullptr) {
    throw std::runtime_error("cannot locate GeoSvc");
    return;
  }

  auto lcdd = m_geoSvc->lcdd();
  auto allReadouts = lcdd->readouts();
  if (allReadouts.find(m_readoutName) == allReadouts.end()) {
    throw std::runtime_error("Readout " + m_readoutName + " not found! Please check tool configuration.");
  } else {
    std::cout << "Reading EDM from the collection " << m_readoutName << std::endl;
  }

  pSeg = dynamic_cast<dd4hep::DDSegmentation::GridDRcalo*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());

  readCSV(m_calibPath);

  if ( static_cast<int>(m_calibs.size()) != pSeg->paramBarrel()->GetTotTowerNum() + pSeg->paramEndcap()->GetTotTowerNum() ) {
    throw std::range_error("Number of calibration constants does not match with the number of towers");
    return;
  }

  m_reader = std::make_unique<podio::ROOTReader>();
  m_reader->openFile(m_filenameIn);

  m_store = std::make_unique<podio::EventStore>();
  m_store->setReader(m_reader.get());

  m_writer = std::make_unique<podio::ROOTWriter>(m_filenameOut, m_store.get());

  m_writer->registerForWrite("SimCalorimeterHits");
  m_writer->registerForWrite("RawCalorimeterHits");
  m_writer->registerForWrite("DRSimCalorimeterHits");
  m_writer->registerForWrite("EventHeader");

  registerForWrite<edm4hep::CalorimeterHitCollection>(m_sHits,"ScintillationHits");
  registerForWrite<edm4hep::CalorimeterHitCollection>(m_cHits,"CherenkovHits");
  registerForWrite<edm4hep::DRrecoCalorimeterHitCollection>(m_DRScintHits,"DRrecoScintillationHits");
  registerForWrite<edm4hep::DRrecoCalorimeterHitCollection>(m_DRCherenHits,"DRrecoCherenkovHits");
  registerForWrite<edm4hep::DRrecoCaloAssociationCollection>(m_scintAssocs,"DRscintAssociations");
  registerForWrite<edm4hep::DRrecoCaloAssociationCollection>(m_cherenAssocs,"DRcherenAssociations");

  std::cout << "RecoStandalone initialized" << std::endl;

  return;
}

void RecoStandalone::execute() {
  auto& inputs_ = m_store->get<edm4hep::DRSimCalorimeterHitCollection>("DRSimCalorimeterHits");
  const edm4hep::DRSimCalorimeterHitCollection* inputs = &inputs_;

  if (!isFirstEvent) {
    create<edm4hep::CalorimeterHitCollection>(m_sHits,"ScintillationHits");
    create<edm4hep::CalorimeterHitCollection>(m_cHits,"CherenkovHits");
    create<edm4hep::DRrecoCalorimeterHitCollection>(m_DRScintHits,"DRrecoScintillationHits");
    create<edm4hep::DRrecoCalorimeterHitCollection>(m_DRCherenHits,"DRrecoCherenkovHits");
    create<edm4hep::DRrecoCaloAssociationCollection>(m_scintAssocs,"DRscintAssociations");
    create<edm4hep::DRrecoCaloAssociationCollection>(m_cherenAssocs,"DRcherenAssociations");
  } else {
    isFirstEvent = false;
  }

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

  edm4hep::CalorimeterHitCollection* scintHit = m_sHits;
  edm4hep::CalorimeterHitCollection* cherenHit = m_cHits;
  edm4hep::DRrecoCalorimeterHitCollection* DRscintHit = m_DRScintHits;
  edm4hep::DRrecoCalorimeterHitCollection* DRcherenHit = m_DRCherenHits;
  edm4hep::DRrecoCaloAssociationCollection* scintAssocs = m_scintAssocs;
  edm4hep::DRrecoCaloAssociationCollection* cherenAssocs = m_cherenAssocs;

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

      add(drHit,hit,input,timeBinCenter,m_calibs.at(absNumEta).first);
    } else {
      auto hit = scintHit->create();
      auto drHit = DRscintHit->create();
      auto assoc = scintAssocs->create();
      assoc.setRec(drHit);
      assoc.setSim(input);

      add(drHit,hit,input,timeBinCenter,m_calibs.at(absNumEta).second);
    }
  }

  return;
}

void RecoStandalone::finalize() {
  m_calibs.clear();
  m_reader->closeFile();
  m_writer->finish();

  return;
}

void RecoStandalone::add(edm4hep::DRrecoCalorimeterHit& drHit, edm4hep::CalorimeterHit& hit, const edm4hep::DRSimCalorimeterHit& input,
                    const std::vector<float>& timeBinCenter, float calib) {
  auto& rawhit = input.getEdm4hepHit();
  auto cID = static_cast<dd4hep::DDSegmentation::CellID>(rawhit.getCellID());

  hit.setEnergy( static_cast<float>(rawhit.getAmplitude())/calib );
  hit.setPosition( getPosition(cID) );
  hit.setCellID( rawhit.getCellID() );
  addToTimeStruct(drHit,input,timeBinCenter,calib);

  drHit.setEdm4hepHit(hit);
}

void RecoStandalone::addToTimeStruct(edm4hep::DRrecoCalorimeterHit& drHit, const edm4hep::DRSimCalorimeterHit& input,
                                const std::vector<float>& timeBinCenter, float calib) {
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

    float numerator = timeBinCenter.at(bin)*dd4hep::nanosecond/CLHEP::nanosecond - std::sqrt(sipmPos.Mag2())/dd4hep::c_light;
    auto pos = sipmPos - ( numerator/invVminusInvC )*fiberUnit;
    edm4hep::Vector3f posEdm(pos.x() * CLHEP::millimeter/dd4hep::millimeter,
                             pos.y() * CLHEP::millimeter/dd4hep::millimeter,
                             pos.z() * CLHEP::millimeter/dd4hep::millimeter);

    drHit.addToTimeStruct(edep);
    drHit.addToPosition( posEdm );
  }
}

edm4hep::Vector3f RecoStandalone::getPosition(dd4hep::DDSegmentation::CellID& cID) {
  auto globalPos = pSeg->position( cID );
  return { static_cast<float>( globalPos.x() * CLHEP::millimeter/dd4hep::millimeter ),
           static_cast<float>( globalPos.y() * CLHEP::millimeter/dd4hep::millimeter ),
           static_cast<float>( globalPos.z() * CLHEP::millimeter/dd4hep::millimeter ) };
}

void RecoStandalone::readCSV(std::string filename) {
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

template <typename T>
void RecoStandalone::create(T*& pCollection, std::string name) {
  auto& collection = m_store->create<T>(name);
  pCollection = &collection;
}

template <typename T>
void RecoStandalone::registerForWrite(T*& pCollection, std::string name) {
  create(pCollection, name);
  m_writer->registerForWrite(name);
}
