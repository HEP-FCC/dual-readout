#include "functions.h"
#include "TLorentzVector.h"

fastjetInterface::fastjetData functions::findSecondary(std::vector<fastjetInterface::fastjetData> vec, double dR) {
  fastjetInterface::fastjetData primary = vec.at(0);
  TLorentzVector primary4vec;
  primary4vec.SetPxPyPzE(primary.px,primary.py,primary.pz,primary.E);

  fastjetInterface::fastjetData secondary;
  for (auto secondary : vec) {
    TLorentzVector secondary4vec;
    secondary4vec.SetPxPyPzE(secondary.px,secondary.py,secondary.pz,secondary.E);
    if ( primary4vec.DeltaR(secondary4vec) > dR ) return secondary;
  }

  fastjet::PseudoJet zero(0.,0.,0.,0.);
  return fastjetInterface::fastjetData(zero);
}

float functions::E_DR(float E_C, float E_S) {
  float hOe_C = 0.2484; float hOe_S = 0.8342;
  float chi = (1.-hOe_S)/(1.-hOe_C);

  return (E_S - chi*E_C)/(1 - chi);
}

float functions::E_DR291(float E_C, float E_S) {
  float chi = 0.291;

  return (E_S - chi*E_C)/(1 - chi);
}

std::vector<fastjetInterface::fastjetData> functions::runFastjet(const std::vector<fastjet::PseudoJet>& input, double dR) {
  fastjet::JetDefinition jetDef(fastjet::ee_genkt_algorithm,dR,-1);

  // Run Fastjet algorithm
  std::vector<fastjet::PseudoJet> inclusiveJets, sortedJets;
  fastjet::ClusterSequence clustSeq(input, jetDef);

  // Extract inclusive jets sorted by pT
  inclusiveJets = clustSeq.inclusive_jets();
  sortedJets    = fastjet::sorted_by_pt(inclusiveJets);

  std::vector<fastjetInterface::fastjetData> output;
  for (auto jet : sortedJets) {
    output.push_back(fastjetInterface::fastjetData(jet));
  }

  return output;
}
