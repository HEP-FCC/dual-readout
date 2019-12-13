#include "fastjetInterface.h"
#include <algorithm>

fastjetInterface::fastjetInterface() {}

fastjetInterface::~fastjetInterface() {}

fastjetInterface::fastjetDataBase::fastjetDataBase(fastjet::PseudoJet& jet) {
  E = jet.E();
  px = jet.px();
  py = jet.py();
  pz = jet.pz();
}

fastjetInterface::fastjetData::fastjetData(fastjet::PseudoJet& jet) {
  E = jet.E();
  px = jet.px();
  py = jet.py();
  pz = jet.pz();
  phi = jet.phi();
  phi_std = jet.phi_std();
  rap = jet.rap();
  eta = jet.eta();
  pt = jet.pt();
  m = jet.m();
  mt = jet.mt();
  hasAssociatedCS = jet.has_associated_cs();
  validCS = jet.has_valid_cs();
  hasConstituents = jet.has_constituents();
  nConstituents = jet.constituents().size();
}

void fastjetInterface::init(TTree* treeIn) {
  treeIn->Branch("Genjets", &fJets);
}

void fastjetInterface::writeJets(std::vector<fastjet::PseudoJet> jets) {
  fJets.clear();
  fJets.reserve(jets.size());

  for (auto jet = jets.begin(); jet != jets.end(); ++jet) {
    fJets.push_back(fastjetData(*jet));
  }
}
