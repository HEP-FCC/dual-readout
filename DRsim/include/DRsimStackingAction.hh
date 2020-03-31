#ifndef DRsimStackingAction_h
#define DRsimStackingAction_h 1

#include "G4UserStackingAction.hh"
#include "G4StackManager.hh"
#include "G4GenericMessenger.hh"

class DRsimStackingAction : public G4UserStackingAction {
public:
  DRsimStackingAction();
  virtual ~DRsimStackingAction() {}

  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* track);

public:
  void transportOp(const G4Step* step);

private:
  void DefineCommands();

  G4GenericMessenger* fMessenger;
  double fSafety;
};

#endif
