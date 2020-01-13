#include "RecoInterface.h"
#include "DRsimCellParameterisation.hh"

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
  IsCerenkov = DRsimCellParameterisation::IsCerenkov(sipmIn.x,sipmIn.y);
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
