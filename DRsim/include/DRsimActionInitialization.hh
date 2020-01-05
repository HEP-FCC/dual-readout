#ifndef DRsimActionInitialization_h
#define DRsimActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "globals.hh"

class DRsimActionInitialization : public G4VUserActionInitialization {
public:
  DRsimActionInitialization(int seed, G4String hepMCpath);
  virtual ~DRsimActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;
private:
  int fSeed;
  G4String fHepMCpath;
};

#endif
