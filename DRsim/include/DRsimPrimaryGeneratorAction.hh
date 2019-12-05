#ifndef DRsimPrimaryGeneratorAction_h
#define DRsimPrimaryGeneratorAction_h 1

#include "globals.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ThreeVector.hh"

#include "HepMCG4AsciiReader.hh"

class G4VPrimaryGenerator;
class G4ParticleGun;
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

private:
  void DefineCommands();

  G4int fSeed;
  G4String fHepMCpath;
  G4ParticleGun* fParticleGun;
  HepMCG4AsciiReader* fHepMCAscii;
  G4GenericMessenger* fMessenger;
  G4ParticleDefinition* fElectron;
  G4ParticleDefinition* fPositron;
  G4ParticleDefinition* fMuon;
  G4ParticleDefinition* fPion;
  G4ParticleDefinition* fKaon;
  G4ParticleDefinition* fProton;
  G4ParticleDefinition* fOptGamma;

  G4double fTheta;
  G4double fPhi;
  G4double fRandX;
  G4double fRandY;

  G4double y;
  G4double fY_0;
  G4double z;
  G4double fZ_0;

  G4ThreeVector org;
  G4ThreeVector direction;
};

#endif
