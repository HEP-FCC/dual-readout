#include "DRsimInterface.h"
#include "RootInterface.h"
#include "fastjetInterface.h"
#include "RecoTower.h"

#include <iostream>

int main(int argc, char* argv[]) {
  std::string filenum = std::string(argv[1]);
  std::string filename = std::string(argv[2]);

  RootInterface<DRsimInterface::DRsimEventData>* drInterface = new RootInterface<DRsimInterface::DRsimEventData>(filename+"_"+filenum+".root");
  drInterface->set("DRsim","DRsimEventData");

  RecoTower* recoTower = new RecoTower();
  recoTower->readCSV();

  unsigned int entries = drInterface->entries();
  while (drInterface->numEvt() < entries) {
    recoTower->clear();
    recoTower->getFiber()->clear();

    DRsimInterface::DRsimEventData evt;
    RecoInterface::RecoEventData recoEvt();
    drInterface->read(evt);

    for (auto towerItr = evt.towers.begin(); towerItr != evt.towers.end(); ++towerItr) {
      auto tower = *towerItr;
      recoTower->reconstruct(tower,recoEvt);

      auto theTower = recoTower->getTower();
      recoEvt.E_C += theTower.E_C;
      recoEvt.E_S += theTower.E_S;
      recoEvt.E_Scorr += theTower.E_Scorr;
      recoEvt.n_C += theTower.n_C;
      recoEvt.n_S += theTower.n_S;
    } // tower loop
    recoEvt.E_DR = RecoTower::E_DR(recoEvt.E_C,recoEvt.E_S);
    recoEvt.E_DRcorr = RecoTower::E_DR(recoEvt.E_C,recoEvt.E_Scorr);

    // // FastJet
    // double dR = 0.4;
    // fastjet::JetDefinition jetDef(fastjet::antikt_algorithm, dR);
    //
    // // Run Fastjet algorithm
    //  inclusiveJets_F, sortedJets_F;
    // std::vector <fastjet::PseudoJet> inclusiveJets_S, sortedJets_S;
    // std::vector <fastjet::PseudoJet> inclusiveJets_DR, sortedJets_DR;
    // fastjet::ClusterSequence clustSeq_F(recoTower->getFiber()->getFjInputs(), jetDef);
    // fastjet::ClusterSequence clustSeq_S(recoTower->getFjInputs_S(), jetDef);
    // fastjet::ClusterSequence clustSeq_DR(recoTower->getFjInputs_DR(), jetDef);
    //
    // // Extract inclusive jets sorted by pT
    // inclusiveJets = clustSeq.inclusive_jets();
    // sortedJets    = fastjet::sorted_by_pt(inclusiveJets);
    //
    // // Write the HepMC event to file. Done with it.
    // fjInterface.writeJets(sortedJets);
    // rootOutput.write_event(*hepmcevt);
    // delete hepmcevt;



  } // event loop

  return 0;
}
