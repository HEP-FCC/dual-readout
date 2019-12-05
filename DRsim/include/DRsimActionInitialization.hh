#ifndef DRsimActionInitialization_h
#define DRsimActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "globals.hh"

class DRsimActionInitialization : public G4VUserActionInitialization {
public:
  DRsimActionInitialization(int seed, G4int wavBin, G4int timeBin, G4String hepMCpath);
  virtual ~DRsimActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;
private:
  int fSeed;
  G4int fWavBin;
  G4int fTimeBin;
  G4String fHepMCpath;
};

#endif
