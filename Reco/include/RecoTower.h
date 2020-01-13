#ifndef RecoTower_h
#define RecoTower_h 1

#include "DRsimInterface.h"
#include "RecoFiber.h"
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

  const std::vector<fastjet::PseudoJet>& getFjInputs_S() { return fFjInputs_S; }
  const std::vector<fastjet::PseudoJet>& getFjInputs_Scorr() { return fFjInputs_Scorr; }
  const std::vector<fastjet::PseudoJet>& getFjInputs_DR() { return fFjInputs_DR; }
  const std::vector<fastjet::PseudoJet>& getFjInputs_DRcorr() { return fFjInputs_DRcorr; }
  void addFjInputs(const RecoInterface::RecoFiberData& recoFiber);
  void clear();

  static float E_DR(float E_C, float E_S);

private:
  int getAbsITheta(const int iTheta);

  RecoFiber* fFiber;
  RecoInterface::RecoTowerData fData;
  std::vector<fastjet::PseudoJet> fFjInputs_S;
  std::vector<fastjet::PseudoJet> fFjInputs_Scorr;
  std::vector<fastjet::PseudoJet> fFjInputs_DR;
  std::vector<fastjet::PseudoJet> fFjInputs_DRcorr;

  std::vector<std::pair<float,float>> fCalibs;
  float fSF_S;
  float fSF_C;
};

#endif
