#include "DRsimInterface.h"

DRsimInterface::DRsimInterface() {}
DRsimInterface::~DRsimInterface() {}

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
