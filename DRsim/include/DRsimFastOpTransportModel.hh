#ifndef DRsimFastOpTransportModel_h
#define DRsimFastOpTransportModel_h 1

#include "G4VFastSimulationModel.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4GenericMessenger.hh"
#include "G4OpBoundaryProcess.hh"

class DRsimFastOpTransportModel : public G4VFastSimulationModel {
public:
  DRsimFastOpTransportModel(G4String, G4Region*);
  ~DRsimFastOpTransportModel();

  virtual G4bool IsApplicable(const G4ParticleDefinition&);
  virtual G4bool ModelTrigger(const G4FastTrack&);
  virtual void DoIt(const G4FastTrack&, G4FastStep&);

  void SetFiberLength(G4double length) { fFiberLength = length; }

private:
  void DefineCommands();

  bool checkTotalInternalReflection(const G4Track* track);
  void setOpBoundaryProc(const G4Track* track);
  void reset();

  G4GenericMessenger* fMessenger;
  G4OpBoundaryProcess* fOpBoundaryProc;
  G4bool fProcAssigned;

  G4double fFiberLength;
  G4int fSafety;
  G4double fTrkLength;
  G4double fNtransport;
  G4double fTransportUnit;
  G4ThreeVector fFiberAxis;
  G4bool fKill;
  G4bool fDoAbsorption;
  G4int fNtotIntRefl;
  G4int fTrackId;
};

#endif
