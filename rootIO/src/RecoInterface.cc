#include "RecoInterface.h"

RecoInterface::RecoTowerData::RecoTowerData(const DRsimInterface::DRsimTowerData& towerIn) {
  iTheta = towerIn.iTheta;
  iPhi = towerIn.iPhi;
  numx = towerIn.numx;
  numy = towerIn.numy;

  E_C = 0.;
  E_S = 0.;
  E_Scorr = 0.;
  E_DR = 0.;
  E_DRcorr = 0.;
  n_C = 0;
  n_S = 0;
}

RecoInterface::RecoFiberData::RecoFiberData(const DRsimInterface::DRsimSiPMData& sipmIn) {
  fiberNum = sipmIn.SiPMnum;
  n = sipmIn.count;

  E = 0.;
  Ecorr = 0.;
  t = 0.;
  depth = 0.;
}

RecoInterface::RecoEventData::RecoEventData() {
  E_C = 0.;
  E_S = 0.;
  E_Scorr = 0.;
  E_DR = 0.;
  E_DRcorr = 0.;
  n_C = 0;
  n_S = 0;
}
