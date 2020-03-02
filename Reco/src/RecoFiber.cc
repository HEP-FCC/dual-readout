#include "TVector3.h"

#include "RecoFiber.h"

#include <cmath>

RecoFiber::RecoFiber() {
  fCalibS = 0.;
  fCalibC = 0.;

  // [L] = mm, [t] = ns
  fSpeed = 158.8;
  fEffSpeedInv = 1./fSpeed - 1./300.;
  fDepthEM = 149.49;
  fAbsLen = 5677.;
  fCThres = 34.1;
}

void RecoFiber::reconstruct(const DRsimInterface::DRsimSiPMData& sipm, RecoInterface::RecoTowerData& recoTower) {
  RecoInterface::RecoFiberData recoFiber(sipm);

  if (recoFiber.IsCerenkov) {
    recoFiber.n = cutXtalk(sipm);
    recoFiber.E = (float)recoFiber.n / fCalibC;
    recoFiber.Ecorr = recoFiber.E;
    recoFiber.t = setTmax(sipm);
    addFjInputs(recoFiber);
  } else {
    recoFiber.E = (float)recoFiber.n / fCalibS;
    recoFiber.t = setTmax(sipm);

    recoFiber.depth = setDepth(recoFiber.t,recoTower);
    recoFiber.Ecorr = recoFiber.E*std::exp((-recoFiber.depth+fDepthEM)/fAbsLen);
    addFjInputs(recoFiber);
  }

  fData = recoFiber;
  recoTower.fibers.push_back(fData);
}

float RecoFiber::setTmax(const DRsimInterface::DRsimSiPMData& sipm) {
  std::pair<DRsimInterface::hitRange,int> maxima = std::make_pair(std::make_pair(0.,0.),0);
  for (auto timeItr = sipm.timeStruct.begin(); timeItr != sipm.timeStruct.end(); ++timeItr) {
    auto timeObj = *timeItr;

    if (timeObj.second > maxima.second) maxima = timeObj;
  }

  return maxima.first.first;
}

float RecoFiber::setDepth(const float tmax, const RecoInterface::RecoTowerData& recoTower) {
  float depth = ( recoTower.innerR/300. + recoTower.towerH/fSpeed - tmax )/( fEffSpeedInv );
  if (depth < 0.) return 0.;
  else if (depth > recoTower.towerH) return recoTower.towerH;
  else return depth;
}

int RecoFiber::cutXtalk(const DRsimInterface::DRsimSiPMData& sipm) {
  int sum = 0;
  for (auto timeItr = sipm.timeStruct.begin(); timeItr != sipm.timeStruct.end(); ++timeItr) {
    auto timeObj = *timeItr;

    if (timeObj.first.first < fCThres) sum += timeObj.second;
  }

  return sum;
}

void RecoFiber::addFjInputs(const RecoInterface::RecoFiberData& recoFiber) {
  TVector3 vec(std::get<0>(recoFiber.pos),std::get<1>(recoFiber.pos),std::get<2>(recoFiber.pos));
  TVector3 p = recoFiber.E*vec.Unit();

  if (recoFiber.IsCerenkov) {
    fFjInputs_C.push_back( fastjet::PseudoJet(p.x(),p.y(),p.z(),recoFiber.E) );
  } else {
    fFjInputs_S.push_back( fastjet::PseudoJet(p.x(),p.y(),p.z(),recoFiber.E) );

    TVector3 p_corr = recoFiber.Ecorr*vec.Unit();
    fFjInputs_Scorr.push_back( fastjet::PseudoJet(p_corr.x(),p_corr.y(),p_corr.z(),recoFiber.Ecorr) );
  }
}

void RecoFiber::clear() {
  fFjInputs_S.clear();
  fFjInputs_Scorr.clear();
  fFjInputs_C.clear();
}
