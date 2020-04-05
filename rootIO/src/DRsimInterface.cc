#include "DRsimInterface.h"

DRsimInterface::DRsimInterface() {}
DRsimInterface::~DRsimInterface() {}

bool DRsimInterface::IsCerenkov(int col, int row) {
  bool isCeren = false;
  if ( col%2 == 1 ) { isCeren = !isCeren; }
  if ( row%2 == 1 ) { isCeren = !isCeren; }
  return isCeren;
}
