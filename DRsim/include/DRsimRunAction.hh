#ifndef DRsimRunAction_h
#define DRsimRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "g4root.hh"

class DRsimEventAction;

class G4Run;

class DRsimRunAction : public G4UserRunAction {
public:
  DRsimRunAction(DRsimEventAction* eventAction, int seed, G4int wavBin, G4int timeBin);
  virtual ~DRsimRunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  DRsimEventAction* fEventAction;
  int fSeed;
  G4int fWavBin;
  G4int fTimeBin;
};

#endif
