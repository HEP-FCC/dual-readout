#include "RecoInterface.h"

RecoInterface::RecoTowerData::RecoTowerData(const DRsimInterface::DRsimTowerData& towerIn) {
  theta = towerIn.towerTheta;
  phi = towerIn.towerPhi;
  numx = towerIn.numx;
  numy = towerIn.numy;
  innerR = towerIn.innerR;
  towerH = towerIn.towerH;
  dTheta = towerIn.dTheta;

  E_C = 0.;
  E_S = 0.;
  E_Scorr = 0.;
  n_C = 0;
  n_S = 0;
  t_C = 0.;
  t_S = 0.;
  depth = 0.;
}
