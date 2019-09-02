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

#include "WGR16PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <cmath>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
using namespace std;
WGR16PrimaryGeneratorAction::WGR16PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0), fMessenger(0),
  fElectron(0), fPositron(0), fMuon(0), fPion(0), fKaon(0), fProton(0),fOptGamma(0)
{
  fTheta = -0.01111;
  fPhi = 0.;
  fRandX = 10.*mm;
  fRandY = 10.*mm;
  fY_0 = 0.;
  fZ_0 = 0.;
  fParticleGun  = new G4ParticleGun(1);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  fElectron = particleTable->FindParticle(particleName="e-");
  fPositron = particleTable->FindParticle(particleName="e+");
  fMuon = particleTable->FindParticle(particleName="mu+");
  fPion = particleTable->FindParticle(particleName="pi+");
  fKaon = particleTable->FindParticle(particleName="kaon+");
  fProton = particleTable->FindParticle(particleName="proton");
  fOptGamma = particleTable->FindParticle(particleName="opticalphoton");

  // define commands for this class
  DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WGR16PrimaryGeneratorAction::~WGR16PrimaryGeneratorAction() {
  delete fParticleGun;
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WGR16PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
  y = (G4UniformRand()-0.5)*fRandX + fY_0;//- 3.142*cm;//
  z = (G4UniformRand()-0.5)*fRandY + fZ_0;//- 4.7135*cm;//10x10 mm^2
  org.set(0,y,z);

  fParticleGun->SetParticlePosition(org); // http://www.apc.univ-paris7.fr/~franco/g4doxy/html/classG4VPrimaryGenerator.html

  direction.setREtaPhi(1.,0.,0.);
  direction.rotateY(fTheta);
  direction.rotateZ(fPhi);

  fParticleGun->SetParticleMomentumDirection(direction);
  fParticleGun->GeneratePrimaryVertex(event);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WGR16PrimaryGeneratorAction::DefineCommands() {
  // Define /WGR16/generator command directory using generic messenger class
  fMessenger = new G4GenericMessenger(this, "/WGR16/generator/", "Primary generator control");

  G4GenericMessenger::Command& etaCmd = fMessenger->DeclareMethodWithUnit("theta","rad",&WGR16PrimaryGeneratorAction::SetTheta,"theta of beam");
  etaCmd.SetParameterName("theta",true);
  etaCmd.SetDefaultValue("-0.01111");

  G4GenericMessenger::Command& phiCmd = fMessenger->DeclareMethodWithUnit("phi","rad",&WGR16PrimaryGeneratorAction::SetPhi,"phi of beam");
  phiCmd.SetParameterName("phi",true);
  phiCmd.SetDefaultValue("0.");

  G4GenericMessenger::Command& y0Cmd = fMessenger->DeclareMethodWithUnit("y0","cm",&WGR16PrimaryGeneratorAction::SetY0,"y_0 of beam");
  y0Cmd.SetParameterName("y0",true);
  y0Cmd.SetDefaultValue("0.");

  G4GenericMessenger::Command& z0Cmd = fMessenger->DeclareMethodWithUnit("z0","cm",&WGR16PrimaryGeneratorAction::SetZ0,"z_0 of beam");
  z0Cmd.SetParameterName("z0",true);
  z0Cmd.SetDefaultValue("0.");

  G4GenericMessenger::Command& randxCmd = fMessenger->DeclareMethodWithUnit("randx","mm",&WGR16PrimaryGeneratorAction::SetRandX,"x width of beam");
  randxCmd.SetParameterName("randx",true);
  randxCmd.SetDefaultValue("10.");

  G4GenericMessenger::Command& randyCmd = fMessenger->DeclareMethodWithUnit("randy","mm",&WGR16PrimaryGeneratorAction::SetRandY,"y width of beam");
  randyCmd.SetParameterName("randy",true);
  randyCmd.SetDefaultValue("10.");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
