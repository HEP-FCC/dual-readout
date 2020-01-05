#ifndef DRsimRunAction_h
#define DRsimRunAction_h 1

#include "DRsimRootInterface.h"
#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

class DRsimRunAction : public G4UserRunAction {
public:
  DRsimRunAction(int seed, G4String hepMCpath);
  virtual ~DRsimRunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

  static DRsimRootInterface* sRootIO;
  static int sNumRef;
  static int sNumEvt;

private:
  int fSeed;
  G4String fHepMCpath;
};

#endif
