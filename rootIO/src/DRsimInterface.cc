#include "DRsimInterface.h"

DRsimInterface::DRsimInterface() {}
DRsimInterface::~DRsimInterface() {}

bool DRsimInterface::IsCerenkov(int col, int row) {
  bool isCeren = false;
  if ( col%2 == 1 ) { isCeren = !isCeren; }
  if ( row%2 == 1 ) { isCeren = !isCeren; }
  return isCeren;
}

DRsimInterface::DRsimEdepData::DRsimEdepData() {
  Edep = 0.;
  EdepEle = 0.;
  EdepGamma = 0.;
  EdepCharged = 0.;
  iTheta = 999;
  iPhi = 999;
}

DRsimInterface::DRsimEdepFiberData::DRsimEdepFiberData() {
  fiberNum = -1;
  x = -1;
  y = -1;
  IsCerenkov = false;
  Edep = 0.;
  EdepEle = 0.;
  EdepGamma = 0.;
  EdepCharged = 0.;
}
