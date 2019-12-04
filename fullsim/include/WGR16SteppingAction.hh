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

#ifndef WGR16SteppingAction_h
#define WGR16SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include <stdio.h>
#include "G4LogicalVolume.hh"
#include "G4Step.hh"
#include "G4TrackVector.hh"
#include <map>

using namespace std;
class WGR16EventAction;
class G4LogicalVolume;

class WGR16SteppingAction : public G4UserSteppingAction {
public:
  WGR16SteppingAction(WGR16EventAction* eventAction);
  virtual ~WGR16SteppingAction();
  virtual void UserSteppingAction(const G4Step*);

private:
  WGR16EventAction* fEventAction;
  G4LogicalVolume* fScoringVolume;

  bool postvolnull;
  G4int fullID;
  G4double pdgCharge;
  G4int pdgID;

  G4LogicalVolume* pre_volume;
  G4StepPoint* presteppoint;
  G4StepPoint* poststeppoint;
  G4TouchableHandle theTouchable;
  G4ParticleDefinition* particle;
  G4Track* track;
  G4float fEdep;
  G4float fEdepEle;
  G4float fEdepGamma;
  G4float fEdepCharged;
  G4VPhysicalVolume* motherTower;
  G4int towerIEta;
  G4int towerIPhi;
  G4double sl;

  std::map<G4String, int> towerNameMap = {
    {"Copper",0}, {"Polystyrene",2}, {"PMMA_Clad",1}, {"PMMA",2}, {"Fluorinated_Polymer",1}, {"Silicon",3}, {"Glass",2}, {"gelatin",2}, {"GlassLayer",1}
  };

  G4VPhysicalVolume* GetMotherTower(G4String matName, G4TouchableHandle touchable) {
    return touchable->GetVolume(towerNameMap[matName]);
  }

  G4int GetTowerIEta(G4String towerName) {
    G4int towerIEta_ = std::stoi(towerName.substr(3));
    bool IsR = (towerName.find("R")==std::string::npos) ? false : true;
    bool IsB = (towerName.find("B")==std::string::npos) ? false : true;
    if(!IsB) towerIEta_ += 52;
    if(!IsR) towerIEta_ = -towerIEta_ -1;

    return towerIEta_;
  }

  G4int GetTowerIPhi(G4VPhysicalVolume* motherTower_) {
    return motherTower_->GetCopyNo();
  }
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
