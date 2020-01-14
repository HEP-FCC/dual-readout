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
  E_DR = 0.;
  E_DRcorr = 0.;
  n_C = 0;
  n_S = 0;
}

RecoInterface::RecoFiberData::RecoFiberData(const DRsimInterface::DRsimSiPMData& sipmIn) {
  IsCerenkov = RecoInterface::IsCerenkov(sipmIn.x,sipmIn.y);
  n = sipmIn.count;
  x = sipmIn.x;
  y = sipmIn.y;
  pos = sipmIn.pos;

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

bool RecoInterface::IsCerenkov(int col, int row) {
  bool isCeren = false;
  if ( col%2 == 1 ) { isCeren = !isCeren; }
  if ( row%2 == 1 ) { isCeren = !isCeren; }
  return isCeren;
}
