#include "DRsimInterface.h"
#include "RootInterface.h"
#include "fastjetInterface.h"
#include "RecoTower.h"

#include <iostream>

int main(int argc, char* argv[]) {
  std::string filenum = std::string(argv[1]);
  std::string filename = std::string(argv[2]);

  RootInterface<RecoInterface::RecoEventData>* recoInterface = new RootInterface<RecoInterface::RecoEventData>(filename+"_"+filenum+".root");
  recoInterface->create("Reco","RecoEventData");

  fastjetInterface fjFiber_S;
  fjFiber_S.init(recoInterface->getTree(),"RecoFiberJets_S");
  fastjetInterface fjFiber_Scorr;
  fjFiber_Scorr.init(recoInterface->getTree(),"RecoFiberJets_Scorr");
  fastjetInterface fjFiber_C;
  fjFiber_C.init(recoInterface->getTree(),"RecoFiberJets_C");

  RootInterface<DRsimInterface::DRsimEventData>* drInterface = new RootInterface<DRsimInterface::DRsimEventData>(filename+"_"+filenum+".root");
  drInterface->set("DRsim","DRsimEventData");

  RecoTower* recoTower = new RecoTower();
  recoTower->readCSV();

  unsigned int entries = drInterface->entries();
  while (drInterface->numEvt() < entries) {
    recoTower->getFiber()->clear();

    DRsimInterface::DRsimEventData evt;
    RecoInterface::RecoEventData* recoEvt = new RecoInterface::RecoEventData();
    drInterface->read(evt);

    for (auto towerItr = evt.towers.begin(); towerItr != evt.towers.end(); ++towerItr) {
      auto tower = *towerItr;
      recoTower->reconstruct(tower,*recoEvt);

      auto theTower = recoTower->getTower();
      recoEvt->E_C += theTower.E_C;
      recoEvt->E_S += theTower.E_S;
      recoEvt->E_Scorr += theTower.E_Scorr;
      recoEvt->n_C += theTower.n_C;
      recoEvt->n_S += theTower.n_S;
    } // tower loop
    recoEvt->E_DR = RecoTower::E_DR(recoEvt->E_C,recoEvt->E_S);
    recoEvt->E_DRcorr = RecoTower::E_DR(recoEvt->E_C,recoEvt->E_Scorr);

    fjFiber_S.runFastjet(recoTower->getFiber()->getFjInputs_S());
    fjFiber_Scorr.runFastjet(recoTower->getFiber()->getFjInputs_Scorr());
    fjFiber_C.runFastjet(recoTower->getFiber()->getFjInputs_C());

    recoInterface->fill(recoEvt);
    delete recoEvt;

  } // event loop

  drInterface->close();
  recoInterface->write();
  recoInterface->close();

  return 0;
}
