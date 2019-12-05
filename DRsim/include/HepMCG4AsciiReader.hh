#ifndef HEPMC_G4_ASCII_READER_H
#define HEPMC_G4_ASCII_READER_H

#include "HepMCG4Interface.hh"
#include "HepMC3/ReaderAsciiHepMC2.h"
#include "HepMC3/Units.h"
#include "HepMC3/Print.h"

class G4GenericMessenger;

class HepMCG4AsciiReader : public HepMCG4Interface {
protected:
  HepMC3::ReaderAsciiHepMC2* asciiInput;

  G4int verbose;

  virtual HepMC3::GenEvent* GenerateHepMCEvent();

public:
  HepMCG4AsciiReader(G4int seed, G4String hepMCpath);
  ~HepMCG4AsciiReader();

  void SetVerboseLevel(G4int i) { verbose = i; }
  G4int GetVerboseLevel() const { return verbose; }

  void Initialize();

private:
  void DefineCommands();

  G4GenericMessenger* fMessenger;
  G4int fSeed;
  G4String fHepMCpath;
};

#endif
