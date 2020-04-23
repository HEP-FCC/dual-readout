#include "fastjetInterface.h"

#include "fastjet/ClusterSequence.hh"
#include "fastjet/PseudoJet.hh"

#include <algorithm>
#include <cmath>

fastjetInterface::fastjetInterface() {
  fJets = new std::vector<fastjetData>(0);
  fJetBase = new std::vector<fastjetDataBase>(0);
}

fastjetInterface::~fastjetInterface() {
  if (fJets) delete fJets;
  if (fJetBase) delete fJetBase;
}

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

  fastjet::PseudoJet childPJ;
  hasChild = jet.has_child(childPJ);
  child = fastjetDataBase(childPJ);
}

void fastjetInterface::init(TTree* treeIn, std::string branchname) {
  treeIn->Branch(branchname.c_str(), &fJets);
}

void fastjetInterface::writeJets(std::vector<fastjet::PseudoJet> jets) {
  fJets->clear();
  fJets->reserve(jets.size());

  for (auto jet = jets.begin(); jet != jets.end(); ++jet) {
    fJets->push_back(fastjetData(*jet));
  }
}

void fastjetInterface::runFastjet(const std::vector<fastjet::PseudoJet>& input) {
  // FastJet
  double dR = 0.8;
  fastjet::JetDefinition jetDef(fastjet::ee_genkt_algorithm,dR,-1);

  // Run Fastjet algorithm
  std::vector<fastjet::PseudoJet> inclusiveJets, sortedJets;
  fastjet::ClusterSequence clustSeq(input, jetDef);

  // Extract inclusive jets sorted by pT
  inclusiveJets = clustSeq.inclusive_jets();
  sortedJets    = fastjet::sorted_by_pt(inclusiveJets);

  writeJets(sortedJets);
}

void fastjetInterface::set(TTree* treeIn, std::string branchname) {
  treeIn->SetBranchAddress(branchname.c_str(),&fJets);
}

void fastjetInterface::read(std::vector<fastjetData>& jets) {
  jets = *fJets;
}
