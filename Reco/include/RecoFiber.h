#ifndef RecoFiber_h
#define RecoFiber_h 1

#include "DRsimInterface.h"
#include "fastjet/PseudoJet.hh"

class RecoFiber {
public:
  RecoFiber();
  ~RecoFiber() {};

  void reconstruct(const DRsimInterface::DRsimSiPMData& sipm, RecoInterface::RecoTowerData& recoTower);
  RecoInterface::RecoFiberData getFiber() { return fData; }

  void setCalibS(float calibS) { fCalibS = calibS; }
  void setCalibC(float calibC) { fCalibC = calibC; }
  const std::vector<fastjet::PseudoJet>& getFjInputs_S() { return fFjInputs_S; }
  const std::vector<fastjet::PseudoJet>& getFjInputs_Scorr() { return fFjInputs_Scorr; }
  void addFjInputs(const RecoInterface::RecoFiberData& recoFiber);
  void clear();

private:
  float setTmax(const DRsimInterface::DRsimSiPMData& sipm);
  float setDepth(const float tmax, const RecoInterface::RecoTowerData& recoTower);

  RecoInterface::RecoFiberData fData;
  std::vector<fastjet::PseudoJet> fFjInputs_S;
  std::vector<fastjet::PseudoJet> fFjInputs_Scorr;

  float fCalibS;
  float fCalibC;

  float fSpeed;
  float fEffSpeedInv;
  float fDepthEM;
  float fAbsLen;
};

#endif
