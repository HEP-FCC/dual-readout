#ifndef FastOpTransportModel_h
#define FastOpTransportModel_h 1

#include "G4VFastSimulationModel.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4GenericMessenger.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4Material.hh"

class FastOpTransportModel : public G4VFastSimulationModel {
public:
  FastOpTransportModel(G4String, G4Region*);
  ~FastOpTransportModel();

  virtual G4bool IsApplicable(const G4ParticleDefinition&);
  virtual G4bool ModelTrigger(const G4FastTrack&);
  virtual void DoIt(const G4FastTrack&, G4FastStep&);

  void SetFiberLength(G4double length) { fFiberLength = length; }
  void SetCoreMaterial(G4Material* mat) { fCoreMaterial = mat; }

private:
  void DefineCommands();

  bool checkTotalInternalReflection(const G4Track* track);
  G4double CalculateVelocityForOpticalPhoton(const G4Track* track);
  void setOpBoundaryProc(const G4Track* track);
  void reset();

  G4GenericMessenger* fMessenger;
  G4OpBoundaryProcess* fOpBoundaryProc;
  G4Material* fCoreMaterial;
  G4bool fProcAssigned;

  G4double fFiberLength;
  G4int fSafety;
  G4double fTrkLength;
  G4double fNtransport;
  G4double fTransportUnit;
  G4ThreeVector fFiberAxis;
  G4bool fKill;
  G4int fNtotIntRefl;
  G4int fTrackId;
};

#endif
