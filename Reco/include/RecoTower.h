#ifndef RecoTower_h
#define RecoTower_h 1

#include "RecoInterface.h"
#include "RecoFiber.h"
#include "DRsimInterface.h"
#include "fastjet/PseudoJet.hh"

#include <utility>
#include <vector>
#include <iostream>

class RecoTower {
public:
  RecoTower();
  ~RecoTower();

  void readCSV(std::string filename="calib.csv");
  void reconstruct(const DRsimInterface::DRsimTowerData& tower, RecoInterface::RecoEventData& evt);
  RecoFiber* getFiber() { return fFiber; }
  RecoInterface::RecoTowerData getTower() { return fData; }

  static float E_DR(float E_C, float E_S);

private:
  int getAbsITheta(const int iTheta);

  RecoFiber* fFiber;
  RecoInterface::RecoTowerData fData;

  std::vector<std::pair<float,float>> fCalibs;
  float fSF_S;
  float fSF_C;
};

#endif
