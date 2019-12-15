#ifndef HEPMC_G4_READER_h
#define HEPMC_G4_READER_h 1

#include "HepMCG4Interface.hh"
#include "HepMC3/ReaderRootTree.h"
#include "HepMC3/Units.h"
#include "HepMC3/Print.h"

class G4GenericMessenger;

class HepMCG4Reader : public HepMCG4Interface {
protected:
  HepMC3::ReaderRootTree* reader;

  G4int verbose;

  virtual HepMC3::GenEvent* GenerateHepMCEvent();

public:
  HepMCG4Reader(G4int seed, G4String hepMCpath);
  ~HepMCG4Reader();

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
