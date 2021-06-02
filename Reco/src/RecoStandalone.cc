#include "RecoStandalone.h"

#include "GridDRcalo.h"

#include "DD4hep/DD4hepUnits.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "TFile.h"
#include "TMatrixTBase.h"
#include "TMatrixDfwd.h"
#include "TDecompSVD.h"

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

  m_scintFit = std::make_unique<TF1>("scintFitFunction",static_cast<TString>(m_funcFormula),m_fitRangeLower,m_fitRangeUpper);
  m_scintFit->SetParameter(0,m_fitParam0);
  m_scintFit->SetParameter(1,m_fitParam1);
  m_scintFit->SetParameter(2,m_fitParam2);
  // m_scintFit->SetParameter(3,m_fitParam3);

  std::cout << "initialized scintFitFunction with formula " << m_funcFormula << std::endl;

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

  return;
}

void RecoStandalone::finalize() {
  m_calibs.clear();
  m_reader->closeFile();
  m_writer->finish();

  return;
}

void RecoStandalone::add(edm4hep::DRrecoCalorimeterHit& drHit, edm4hep::CalorimeterHit& hit, const edm4hep::DRSimCalorimeterHit& input, const float calib) {
  auto& rawhit = input.getEdm4hepHit();
  auto cID = static_cast<dd4hep::DDSegmentation::CellID>(rawhit.getCellID());

  hit.setEnergy( static_cast<float>(rawhit.getAmplitude())/calib );
  hit.setPosition( getPosition(cID) );
  hit.setCellID( rawhit.getCellID() );
  addToTimeStruct(drHit,input,calib);

  drHit.setEdm4hepHit(hit);
}

void RecoStandalone::addToTimeStruct(edm4hep::DRrecoCalorimeterHit& drHit, const edm4hep::DRSimCalorimeterHit& input, const float calib) {
  auto cID = static_cast<dd4hep::DDSegmentation::CellID>(input.getEdm4hepHit().getCellID());
  int numPhi = pSeg->numPhi( cID );
  auto towerPos = pParamBase->GetTowerPos(numPhi);
  auto waferPos = pParamBase->GetSipmLayerPos(numPhi);
  auto sipmPos_ = pSeg->position(cID);
  dd4hep::Position sipmPos(sipmPos_.x(),sipmPos_.y(),sipmPos_.z()); // type cast from dd4hep::DDSegmentation::Vector3D to dd4heo::Position

  auto fiberDir = waferPos - towerPos; // outward direction
  auto fiberUnit = fiberDir.Unit();

  if (pSeg->IsCerenkov(cID)) processCheren(drHit,input,calib,sipmPos,fiberUnit);
  else processScint(drHit,input,calib,sipmPos,fiberUnit);
}

void RecoStandalone::processCheren(edm4hep::DRrecoCalorimeterHit& drHit, const edm4hep::DRSimCalorimeterHit& input,
                                   const float calib, const dd4hep::Position& sipmPos, const dd4hep::Position& fiberUnit) {
  auto& rawhit = input.getEdm4hepHit();

  if (rawhit.getAmplitude() < m_minCheren) {
    drHit.setValidTimeStruct(0); // too few statistics

    return;
  }

  float effVelocity = m_cherenSpeed*dd4hep::millimeter/dd4hep::nanosecond;
  float invVminusInvC = 1./effVelocity - 1./dd4hep::c_light;

  timeEnergyMap timeStruct_postprocess; // time, energy
  float totEdep = static_cast<float>(rawhit.getAmplitude())/calib;
  float esum = processThreshold(input,calib,timeStruct_postprocess);

  for (auto ts = timeStruct_postprocess.begin(); ts != timeStruct_postprocess.end(); ++ts) {
    float numerator = ts->first - std::sqrt(sipmPos.Mag2())/dd4hep::c_light;
    auto pos = sipmPos - ( numerator/invVminusInvC )*fiberUnit;

    if (!pruneTail(pos,fiberUnit)) continue;

    edm4hep::Vector3f posEdm(pos.x() * CLHEP::millimeter/dd4hep::millimeter,
                             pos.y() * CLHEP::millimeter/dd4hep::millimeter,
                             pos.z() * CLHEP::millimeter/dd4hep::millimeter);

    drHit.addToTimeStruct( (ts->second)*totEdep/esum );
    drHit.addToPosition( posEdm );
  }

  drHit.setValidTimeStruct(2); // ok
}

void RecoStandalone::processScint(edm4hep::DRrecoCalorimeterHit& drHit, const edm4hep::DRSimCalorimeterHit& input,
                                  const float calib, const dd4hep::Position& sipmPos, const dd4hep::Position& fiberUnit) {
  auto& rawhit = input.getEdm4hepHit();

  if (rawhit.getAmplitude() < m_minScint) {
    drHit.setValidTimeStruct(0); // too few statistics

    return;
  }

  float effVelocity = m_scintSpeed*dd4hep::millimeter/dd4hep::nanosecond;
  float invVminusInvC = 1./effVelocity - 1./dd4hep::c_light;

  timeEnergyMap timeStruct_postprocess; // time, energy
  float totEdep = static_cast<float>(rawhit.getAmplitude())/calib;
  float esum = processThreshold(input,calib,timeStruct_postprocess);

  std::vector<double> sourceArr;
  sourceArr.reserve(timeStruct_postprocess.size());
  std::vector<double> kernelInit(timeStruct_postprocess.size()*timeStruct_postprocess.size(),0.);

  double kernelPeak = 0.;
  TMatrixD kernelMat(timeStruct_postprocess.size(),timeStruct_postprocess.size(),&(kernelInit[0]));

  for (auto row = timeStruct_postprocess.begin(); row != timeStruct_postprocess.end(); ++row) {
    sourceArr.emplace_back(row->second);
    std::vector<double> arr;
    arr.reserve(timeStruct_postprocess.size());
    for (auto col = timeStruct_postprocess.begin(); col != timeStruct_postprocess.end(); ++col) {
      m_scintFit->SetParameter(0,col->first/dd4hep::nanosecond); // WARNING units in [ns]!
      double eval = m_scintFit->Eval(row->first/dd4hep::nanosecond);
      arr.emplace_back(eval);
      kernelPeak = std::max(kernelPeak,eval);
    }
    TMatrixD sub(1,timeStruct_postprocess.size(),&(arr[0]));
    kernelMat.SetSub( std::distance(timeStruct_postprocess.begin(),row), 0, sub );
  }

  TVectorD sourceVec(timeStruct_postprocess.size(),&(sourceArr[0]));
  sourceVec *= 1./sourceVec.Max(); // normalize to 1 at peak

  TDecompSVD sol(kernelMat);
  sol.SetTol(m_svdTolerance*kernelPeak);
  bool isOk = false;
  auto targetVec = sol.Solve(sourceVec,isOk); // solve sourceVec = kernelMat*targetVec
  double* targetArr = targetVec.GetMatrixArray();

  if (verbose > 1)
    std::cout << "SVD decomposition is (OK / not OK) = " << isOk << ", condition # is " << sol.Condition() << std::endl;

  timeEnergyMap target_postprocess;
  float targetsum = 0.;
  int validness = 1; // at least we have enough statistics

  if (isOk) {
    double targetMax = targetVec.Max();
    timeEnergyMap target_preprocess;

    for (auto itr = timeStruct_postprocess.begin(); itr != timeStruct_postprocess.end(); ++itr) {
      float time_ = itr->first;
      target_preprocess.insert(std::make_pair(time_,static_cast<float>(targetArr[ std::distance(timeStruct_postprocess.begin(),itr) ])));
    }

    targetsum = processThreshold(targetMax,target_preprocess,target_postprocess); // cut threshold once more
    validness = 2;
  } else {
    target_postprocess = timeStruct_postprocess; // fall back to original distribution
    targetsum = esum;
  }

  for (auto ts = target_postprocess.begin(); ts != target_postprocess.end(); ++ts) {
    float numerator = ts->first - std::sqrt(sipmPos.Mag2())/dd4hep::c_light;
    auto pos = sipmPos - ( numerator/invVminusInvC )*fiberUnit;

    if (!pruneTail(pos,fiberUnit)) continue;

    edm4hep::Vector3f posEdm(pos.x() * CLHEP::millimeter/dd4hep::millimeter,
                             pos.y() * CLHEP::millimeter/dd4hep::millimeter,
                             pos.z() * CLHEP::millimeter/dd4hep::millimeter);

    drHit.addToTimeStruct( (ts->second)*totEdep/targetsum );
    drHit.addToPosition( posEdm );
  }

  drHit.setValidTimeStruct(validness);
}

float RecoStandalone::processThreshold(const edm4hep::DRSimCalorimeterHit& input, const float calib, timeEnergyMap& timeStruct_postprocess) {
  float peak = 0.;
  timeEnergyMap timeStruct_preprocess; // time, energy

  for (unsigned int bin = 0; bin < input.timeStruct_size(); bin++) {
    float edep = static_cast<float>( input.getTimeStruct(bin) )/calib;
    float timeBin = (input.getTimeBegin(bin)+input.getTimeEnd(bin))*dd4hep::nanosecond/2.; // WARNING caution for unit!
    timeStruct_preprocess.insert(std::make_pair(timeBin,edep));
    peak = std::max(peak,edep);
  }

  float esum = 0.;
  for (auto ts = timeStruct_preprocess.begin(); ts != timeStruct_preprocess.end(); ++ts) {
    if ( ts->second < m_ampThres*peak ) continue;
    timeStruct_postprocess.insert(*ts);
    esum += ts->second;
  }

  return esum;
}

float RecoStandalone::processThreshold(const double targetMax, timeEnergyMap& target_preprocess, timeEnergyMap& target_postprocess) {
  float targetsum = 0.;
  for (auto ts = target_preprocess.begin(); ts != target_preprocess.end(); ++ts) {
    if ( std::abs(ts->second) < m_ampThres*targetMax ) continue; // transformed target can be negative
    target_postprocess.insert(*ts);
    targetsum += ts->second;
  }

  return targetsum;
}

bool RecoStandalone::pruneTail(const dd4hep::Position& pos, const dd4hep::Position& fiberUnit) {
  double innerProduct = pos.Dot(fiberUnit);
  return innerProduct > pParamBase->GetCurrentInnerR() - m_pruneTolerance*dd4hep::millimeter;
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
