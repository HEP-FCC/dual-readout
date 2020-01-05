#ifndef DRsimPrimaryGeneratorAction_h
#define DRsimPrimaryGeneratorAction_h 1

#include "HepMCG4Reader.hh"

#include "globals.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;

class DRsimPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  DRsimPrimaryGeneratorAction(G4int seed, G4String hepMCpath);
  virtual ~DRsimPrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event*);

  void SetTheta(G4double theta) { fTheta = theta; }
  G4double GetTheta() const { return fTheta; }

  void SetPhi(G4double phi) { fPhi = phi; }

  void SetY0(G4double y0) { fY_0 = y0; }
  void SetZ0(G4double z0) { fZ_0 = z0; }

  void SetRandX(G4double randx) { fRandX = randx; }
  void SetRandY(G4double randy) { fRandY = randy; }

  static G4ThreadLocal int sIdxEvt;
  static int sNumEvt;

private:
  void DefineCommands();
  void initPtcGun();

  static HepMCG4Reader* sHepMCreader;

  G4int fSeed;
  G4String fHepMCpath;
  G4ParticleGun* fParticleGun;
  G4GenericMessenger* fMessenger;
  G4ParticleDefinition* fElectron;
  G4ParticleDefinition* fPositron;
  G4ParticleDefinition* fMuon;
  G4ParticleDefinition* fPion;
  G4ParticleDefinition* fKaon0L;
  G4ParticleDefinition* fProton;
  G4ParticleDefinition* fOptGamma;

  G4double fTheta;
  G4double fPhi;
  G4double fRandX;
  G4double fRandY;

  G4double fY_0;
  G4double fZ_0;

  G4ThreeVector fOrg;
  G4ThreeVector fDirection;
};

#endif
