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
  iTheta = -999;
  iPhi = -999;
}

DRsimInterface::DRsimEdepData::DRsimEdepData(int theta, int phi, float edep, float edepEle, float edepGamma, float edepCharged) {
  iTheta = theta;
  iPhi = phi;
  Edep = edep;
  EdepEle = edepEle;
  EdepGamma = edepGamma;
  EdepCharged = edepCharged;
}

void DRsimInterface::DRsimEdepData::accumulate(float edep, float edepEle, float edepGamma, float edepCharged) {
  Edep += edep;
  EdepEle += edepEle;
  EdepGamma += edepGamma;
  EdepCharged += edepCharged;
}

DRsimInterface::DRsimEdepFiberData::DRsimEdepFiberData() {
  fiberNum = -1;
  Edep = 0.;
  EdepEle = 0.;
  EdepGamma = 0.;
  EdepCharged = 0.;
}

DRsimInterface::DRsimEdepFiberData::DRsimEdepFiberData(long long int fiberId64, float edep, float edepEle, float edepGamma, float edepCharged) {
  fiberNum = fiberId64;
  Edep = edep;
  EdepEle = edepEle;
  EdepGamma = edepGamma;
  EdepCharged = edepCharged;
}

void DRsimInterface::DRsimEdepFiberData::accumulate(float edep, float edepEle, float edepGamma, float edepCharged) {
  Edep += edep;
  EdepEle += edepEle;
  EdepGamma += edepGamma;
  EdepCharged += edepCharged;
}

void DRsimInterface::DRsimEventData::clear() {
  event_number = 0;
  towers.clear();
  Edeps.clear();
  leaks.clear();
  GenPtcs.clear();
}
