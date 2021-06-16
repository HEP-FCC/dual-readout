#include "DigiPodio.h"

#include <iostream>
#include <stdexcept>

int main(int , char* argv[]) {
  std::string filenameIn = std::string(argv[1]);
  std::string filenameOut = std::string(argv[2]);

  auto digipodio = std::make_unique<drc::DigiPodio>(filenameIn,filenameOut);

  digipodio->initialize();

  unsigned int entries = digipodio->GetReader()->getEntries();
  for (unsigned int iEvt = 0; iEvt < entries; iEvt++) {
    std::cout << "processing " << iEvt << "th event" << std::endl;
    digipodio->execute();

    digipodio->GetWriter()->writeEvent();
    digipodio->GetStore()->clear();
    digipodio->GetReader()->endOfEvent();
  }

  digipodio->finalize();

  return 0;
}
