#ifndef functions_h
#define functions_h 1

#include "fastjetInterface.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include <vector>

namespace functions {
  fastjetInterface::fastjetData findSecondary(std::vector<fastjetInterface::fastjetData> vec, double dR);

  float E_DR(float E_C, float E_S);
  float E_DR291(float E_C, float E_S);

  std::vector<fastjetInterface::fastjetData> runFastjet(const std::vector<fastjet::PseudoJet>& input, double dR);
}

#endif
