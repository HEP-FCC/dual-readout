///////////////////////////////////////////////////////////////////////
//// <CEPC>                                                        ////
//// Wedge Geometry for Dual-reaout calorimter                     ////
////                                                               ////
//// Original Author: Mr.Jo Hyunsuk, Kyunpook National University  ////
////                  Sanghyun Ko, Seoul National University       ////
//// E-Mail: hyunsuk.jo@cern.ch	                                   ////
////         sang.hyun.ko@cern.ch                                  ////
////                                                               ////
///////////////////////////////////////////////////////////////////////
//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************

#ifndef WGR16PrimaryGeneratorAction_h
#define WGR16PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

class G4ParticleGun;
class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;

class WGR16PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  WGR16PrimaryGeneratorAction();
  virtual ~WGR16PrimaryGeneratorAction();

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

  G4ParticleGun* fParticleGun;
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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
