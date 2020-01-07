#include "DRsimInterface.h"
#include "DRsimRootInterface.h"
#include "fastjetInterface.h"
#include "RecoTower.h"

#include <iostream>

int main(int argc, char* argv[]) {
  std::string filenum = std::string(argv[1]);
  std::string filename = std::string(argv[2]);

  DRsimRootInterface* drInterface = new DRsimRootInterface(filename+"_"+filenum+".root");
  drInterface->set();

  RecoTower* recoTower = new RecoTower();
  std::vector<std::pair<float,float>> calibs;
  recoTower->readCSV(calibs);

  unsigned int entries = drInterface->entries();
  while (drInterface->numEvt() < entries) {
    DRsimInterface::DRsimEventData evt;
    drInterface->read(evt);

    for (auto towerItr = evt.towers.begin(); towerItr != evt.towers.end(); ++towerItr) {
      auto tower = *towerItr;
      for (auto sipmItr = tower.SiPMs.begin(); sipmItr != tower.SiPMs.end(); ++sipmItr) {
        auto sipm = *sipmItr;
        for (auto timeItr = sipm.timeStruct.begin(); timeItr != sipm.timeStruct.end(); ++timeItr) {
          auto timeObj = *timeItr;

        }


      } // sipm loop
    } // tower loop
  } // event loop

  return 0;
}
