#include "DRsimInterface.h"

DRsimInterface::DRsimInterface() {}
DRsimInterface::~DRsimInterface() {}

const DRsimInterface::DRsimEventData& DRsimInterface::DRsimEventData::operator=(const DRsimEventData& right) {
  event_number = right.event_number;
  towers = right.towers;
  Edeps = right.Edeps;
  leaks = right.leaks;
  GenPtcs = right.GenPtcs;

  return *this;
}
