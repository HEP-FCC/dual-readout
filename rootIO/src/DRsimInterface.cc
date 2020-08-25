#include "DRsimInterface.h"

DRsimInterface::DRsimInterface() {}
DRsimInterface::~DRsimInterface() {}

DRsimInterface::DRsimEdepData::DRsimEdepData() {
  Edep = 0.;
  EdepEle = 0.;
  EdepGamma = 0.;
  EdepCharged = 0.;
  iTheta = -999;
  iPhi = -999;
}

DRsimInterface::DRsimEdepData::DRsimEdepData(int theta, int phi) {
  iTheta = theta;
  iPhi = phi;
  Edep = 0.;
  EdepEle = 0.;
  EdepGamma = 0.;
  EdepCharged = 0.;
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

DRsimInterface::DRsimEdepFiberData::DRsimEdepFiberData(long long int fiberId64) {
  fiberNum = fiberId64;
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
