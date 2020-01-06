#include "DRsimInterface.h"
#include "DRsimRootInterface.h"
#include "fastjetInterface.h"

#include <iostream>

int main(int argc, char* argv[]) {
  std::string filenum = std::string(argv[1]);
  std::string filename = std::string(argv[2]);

  DRsimRootInterface* drInterface = new DRsimRootInterface(filename+"_"+filenum+".root");
  drInterface->set();

  unsigned int entries = drInterface->entries();
  while (drInterface->numEvt() < entries) {
    DRsimInterface::DRsimEventData evt;
    drInterface->read(evt);

    for (auto tower = evt.towers.begin(); tower != evt.towers.end(); ++tower) {
      for (auto sipm = tower.SiPMs.begin(); tower != tower.SiPMs.end(); ++sipm) {
        /* code */
      } // sipm loop
    } // tower loop
  } // event loop

  return 0;
}
