#ifndef DRsimActionInitialization_h
#define DRsimActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "globals.hh"

class G4GenericMessenger;

class DRsimActionInitialization : public G4VUserActionInitialization {
public:
  DRsimActionInitialization(G4int seed, G4String filename);
  virtual ~DRsimActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;

private:
  void DefineCommands();

  G4GenericMessenger* fMessenger;
  G4int fSeed;
  G4String fFilename;
  G4bool fUseHepMC;
  G4bool fUseCalib;
  G4bool fUseGPS;
};

#endif
