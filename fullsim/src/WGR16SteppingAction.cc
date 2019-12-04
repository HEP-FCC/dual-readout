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

#include "WGR16SteppingAction.hh"
#include "WGR16EventAction.hh"
#include "WGR16DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WGR16SteppingAction::WGR16SteppingAction(WGR16EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fScoringVolume(0),
  fEdep(0.), fEdepEle(0.), fEdepGamma(0.), fEdepCharged(0.)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WGR16SteppingAction::~WGR16SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WGR16SteppingAction::UserSteppingAction(const G4Step* step)
{
  particle = step->GetTrack()->GetDefinition();
  if (particle == G4OpticalPhoton::OpticalPhotonDefinition()) return;

  pre_volume = step->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume();
  presteppoint = step->GetPreStepPoint();
  poststeppoint = step->GetPostStepPoint();
  theTouchable = presteppoint->GetTouchableHandle();

  if (poststeppoint->GetStepStatus() == fWorldBoundary) {
    fEventAction->AddLeakage(pdgID,step->GetPreStepPoint()->GetKineticEnergy(),step->GetPreStepPoint()->GetTotalEnergy());
  }

  G4String matName = pre_volume->GetMaterial()->GetName();

  if ( matName=="G4_Galactic" || matName=="Air" ) return;

  motherTower = GetMotherTower(matName,theTouchable);
  towerIEta = GetTowerIEta(motherTower->GetName());
  towerIPhi = GetTowerIPhi(motherTower);

  pdgID = particle->GetPDGEncoding();
  pdgCharge = particle->GetPDGCharge();
  fEdep = step->GetTotalEnergyDeposit();
  fEdepEle = (std::abs(pdgID)==11) ? fEdep : 0.;
  fEdepGamma = (std::abs(pdgID)==22) ? fEdep : 0.;
  fEdepCharged = ( std::round(std::abs(pdgCharge)) != 0. ) ? fEdep : 0.;
  sl = step->GetStepLength();

  if ( fEdep > 0. ) {
    fEventAction->AddStepInfo(fEdep,fEdepEle,fEdepGamma,fEdepCharged,towerIEta,towerIPhi);
  }

  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
