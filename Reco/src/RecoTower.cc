#include "TVector3.h"

#include "RecoTower.h"
#include "RecoInterface.h"

#include "Riostream.h"

RecoTower::RecoTower() {
  fFiber = new RecoFiber();
  fSF_C = 0.886;
  fSF_S = 0.8815;
}

RecoTower::~RecoTower() {
  if (fFiber) delete fFiber;
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

  if (getAbsITheta(recoTower.theta.first) > 30) return;

  fFiber->setCalibC( fSF_C*fCalibs.at(getAbsITheta(recoTower.theta.first)).first );
  fFiber->setCalibS( fSF_S*fCalibs.at(getAbsITheta(recoTower.theta.first)).second );

  for (auto sipmItr = tower.SiPMs.begin(); sipmItr != tower.SiPMs.end(); ++sipmItr) {
    auto sipm = *sipmItr;

    fFiber->reconstruct(sipm,recoTower);

    auto theFiber = fFiber->getFiber();
    if (theFiber.IsCerenkov) {
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

  addFjInputs(recoTower);
  fData = recoTower;
  evt.towers.push_back(fData);
}

int RecoTower::getAbsITheta(const int iTheta) {
  return iTheta >= 0 ? iTheta : -iTheta -1;
}

void RecoTower::addFjInputs(const RecoInterface::RecoTowerData& recoTower) {
  TVector3 p_DR;
  p_DR.SetMagThetaPhi(recoTower.E_DR,recoTower.theta.second,recoTower.phi.second);
  fFjInputs_DR.push_back( fastjet::PseudoJet(p_DR.x(),p_DR.y(),p_DR.z(),recoTower.E_DR) );

  TVector3 p_DRcorr;
  p_DRcorr.SetMagThetaPhi(recoTower.E_DRcorr,recoTower.theta.second,recoTower.phi.second);
  fFjInputs_DRcorr.push_back( fastjet::PseudoJet(p_DRcorr.x(),p_DRcorr.y(),p_DRcorr.z(),recoTower.E_DRcorr) );

  TVector3 p_S;
  p_S.SetMagThetaPhi(recoTower.E_S,recoTower.theta.second,recoTower.phi.second);
  fFjInputs_S.push_back( fastjet::PseudoJet(p_S.x(),p_S.y(),p_S.z(),recoTower.E_S) );

  TVector3 p_Scorr;
  p_Scorr.SetMagThetaPhi(recoTower.E_Scorr,recoTower.theta.second,recoTower.phi.second);
  fFjInputs_Scorr.push_back( fastjet::PseudoJet(p_Scorr.x(),p_Scorr.y(),p_Scorr.z(),recoTower.E_Scorr) );
}

void RecoTower::clear() {
  fFjInputs_S.clear();
  fFjInputs_Scorr.clear();
  fFjInputs_DR.clear();
  fFjInputs_DRcorr.clear();
}

float RecoTower::E_DR(float E_C, float E_S) {
  float hOe_C = 0.2484; float hOe_S = 0.8342;
  float chi = (1.-hOe_S)/(1.-hOe_C);

  return (E_S - chi*E_C)/(1 - chi);
}
