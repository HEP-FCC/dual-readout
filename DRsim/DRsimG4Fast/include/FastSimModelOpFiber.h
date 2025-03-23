#ifndef FastSimModelOpFiber_h
#define FastSimModelOpFiber_h 1

#include "G4GenericMessenger.hh"
#include "G4Material.hh"
#include "G4OpAbsorption.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpWLS.hh"
#include "G4VFastSimulationModel.hh"

struct FastFiberData {
public:
  FastFiberData(G4int, G4double, G4double, G4double, G4ThreeVector, G4ThreeVector, G4ThreeVector,
                G4int status = G4OpBoundaryProcessStatus::Undefined);
  ~FastFiberData() {}

  void reset();

  G4double GetAbsorptionNILL() { return mOpAbsorptionNumIntLenLeft; }
  void SetAbsorptionNILL(G4double in) { mOpAbsorptionNumIntLenLeft = in; }

  G4double GetWLSNILL() { return mOpWLSNumIntLenLeft; }
  void SetWLSNILL(G4double in) { mOpWLSNumIntLenLeft = in; }

  G4int GetOpBoundaryStatus() { return mOpBoundaryStatus; }
  void SetOpBoundaryStatus(G4int in) { mOpBoundaryStatus = in; }

  G4double GetStepLengthInterval() { return mStepLengthInterval; }
  void AddStepLengthInterval(G4double in) { mStepLengthInterval += in; }

  G4bool checkRepetitive(const FastFiberData, G4bool checkInterval = true);

  G4int trackID;
  G4double kineticEnergy;
  G4double globalTime;
  G4double pathLength;
  G4ThreeVector globalPosition;
  G4ThreeVector momentumDirection;
  G4ThreeVector polarization;

private:
  G4int mOpBoundaryStatus;
  G4double mOpAbsorptionNumIntLenLeft;
  G4double mOpWLSNumIntLenLeft;
  G4double mStepLengthInterval;
};

class FastSimModelOpFiber : public G4VFastSimulationModel {
public:
  FastSimModelOpFiber(G4String, G4Region*);
  ~FastSimModelOpFiber();

  virtual G4bool IsApplicable(const G4ParticleDefinition&);
  virtual G4bool ModelTrigger(const G4FastTrack&);
  virtual void DoIt(const G4FastTrack&, G4FastStep&);

private:
  void DefineCommands();

  G4bool checkTotalInternalReflection(const G4Track* track);
  G4bool checkAbsorption(const G4double prevNILL, const G4double currentNILL);
  G4bool checkNILL();

  void setPostStepProc(const G4Track* track);
  void reset();
  void print();

  G4GenericMessenger* mMessenger;
  G4OpBoundaryProcess* pOpBoundaryProc;
  G4OpAbsorption* pOpAbsorption;
  G4OpWLS* pOpWLS;
  G4Material* pCoreMaterial;
  G4bool fProcAssigned;

  FastFiberData mDataPrevious;
  FastFiberData mDataCurrent;

  G4int fSafety;
  G4double mNtransport;
  G4double mTransportUnit;
  G4ThreeVector mFiberPos;
  G4ThreeVector mFiberAxis;
  G4bool fKill;
  G4bool fTransported;

  G4bool fSwitch;
  G4int fVerbose;
};

#endif
