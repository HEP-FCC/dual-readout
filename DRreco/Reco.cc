#include "RecoStandalone.h"

#include "GeoSvc.h"

#include <iostream>
#include <stdexcept>

int main(int , char* argv[]) {
  std::string filenameIn = std::string(argv[1]);
  std::string filenameOut = std::string(argv[2]);

  new GeoSvc({"./bin/compact/DRcalo.xml"});

  auto recoStd = std::make_unique<RecoStandalone>(filenameIn,filenameOut);

  recoStd->initialize();

  unsigned int entries = recoStd->GetReader()->getEntries();
  for (unsigned int iEvt = 0; iEvt < entries; iEvt++) {
    std::cout << "processing " << iEvt << "th event" << std::endl;
    recoStd->execute();

    recoStd->GetWriter()->writeEvent();
    recoStd->GetStore()->clear();
    recoStd->GetReader()->endOfEvent();
  }

  recoStd->finalize();

  return 0;
}
