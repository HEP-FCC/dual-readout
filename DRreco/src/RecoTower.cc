#include "TVector3.h"

#include "RecoTower.h"
#include "RecoInterface.h"

#include "Riostream.h"

RecoTower::RecoTower() {
  m_geoSvc = GeoSvc::GetInstance();
  m_readoutName = "DRcaloSiPMreadout";

  initialize();

  fFiber = new RecoFiber();
  fFiber->setSegmentation(fSeg);
  fSF_C = 1.;
  fSF_S = 1.;
}

RecoTower::~RecoTower() {
  if (fFiber) delete fFiber;
}

void RecoTower::initialize() {
  auto lcdd = m_geoSvc->lcdd();
  auto allReadouts = lcdd->readouts();
  if (allReadouts.find(m_readoutName) == allReadouts.end()) {
    throw std::runtime_error("Readout " + m_readoutName + " not found! Please check tool configuration.");
  } else {
    std::cout << "Reading EDM from the collection " << m_readoutName << std::endl;
  }

  fSeg = dynamic_cast<dd4hep::DDSegmentation::GridDRcalo*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());

  return;
}

void RecoTower::readCSV(std::string filename) {
  std::ifstream in;
  int i;
  float ceren, scint;

  in.open(filename,std::ios::in);
  while (true) {
    in >> i >> ceren >> scint;
    if (!in.good()) break;
    fCalibs.push_back(std::make_pair(ceren,scint));
  }
  in.close();
}

void RecoTower::reconstruct(const DRsimInterface::DRsimTowerData& tower, RecoInterface::RecoEventData& evt) {
  RecoInterface::RecoTowerData recoTower(tower);

  if (getAbsITheta(recoTower.iTheta) >= (int)fCalibs.size()) {
    fData = recoTower;
    return;
  }

  fFiber->setCalibC( fSF_C*fCalibs.at(getAbsITheta(recoTower.iTheta)).first );
  fFiber->setCalibS( fSF_S*fCalibs.at(getAbsITheta(recoTower.iTheta)).second );

  for (auto sipmItr = tower.SiPMs.begin(); sipmItr != tower.SiPMs.end(); ++sipmItr) {
    auto sipm = *sipmItr;

    fFiber->reconstruct(sipm,recoTower);

    auto theFiber = fFiber->getFiber();
    if (fSeg->IsCerenkov(theFiber.fiberNum)) {
      recoTower.E_C += theFiber.E;
      recoTower.n_C += theFiber.n;
    } else {
      recoTower.E_S += theFiber.E;
      recoTower.E_Scorr += theFiber.Ecorr;
      recoTower.n_S += theFiber.n;
    }
  }

  recoTower.E_DR = E_DR(recoTower.E_C,recoTower.E_S);
  recoTower.E_DRcorr = E_DR(recoTower.E_C,recoTower.E_Scorr);

  fData = recoTower;
  evt.towers.push_back(fData);
}

int RecoTower::getAbsITheta(const int iTheta) {
  return iTheta >= 0 ? iTheta : -iTheta -1;
}

float RecoTower::E_DR(float E_C, float E_S) {
  float hOe_C = 0.2484; float hOe_S = 0.8342;
  float chi = (1.-hOe_S)/(1.-hOe_C);

  return (E_S - chi*E_C)/(1 - chi);
}
