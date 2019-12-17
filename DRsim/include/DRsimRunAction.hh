#ifndef DRsimRunAction_h
#define DRsimRunAction_h 1

#include "G4UserRunAction.hh"

class G4Run;

class DRsimRunAction : public G4UserRunAction {
public:
  DRsimRunAction(int seed, G4int wavBin, G4int timeBin);
  virtual ~DRsimRunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  int fSeed;
  G4int fWavBin;
  G4int fTimeBin;
};

#endif
