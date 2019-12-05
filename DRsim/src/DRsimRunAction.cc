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

#include "WGR16DetectorConstruction.hh"
#include "WGR16RunAction.hh"
#include "WGR16EventAction.hh"
#include "WGR16PrimaryGeneratorAction.hh"
#include "G4Run.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"

#include <vector>
using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WGR16RunAction::WGR16RunAction(WGR16EventAction* eventAction, int seed, G4int wavBin, G4int timeBin)
: G4UserRunAction(), fEventAction(eventAction)
{
  fSeed = seed;
  fWavBin = wavBin;
  fTimeBin = timeBin;
  G4AnalysisManager* aM = G4AnalysisManager::Instance();
  G4cout << "Using " << aM->GetType() << G4endl;

  char outputname[128]; std::sprintf(outputname,"dr_seednum%d",fSeed);

  // Default settings
  aM->SetVerboseLevel(1);
  aM->SetNtupleMerging(true);
  aM->SetFileName(outputname);

  // Creating ntuple

  if ( fEventAction ) {
    aM->CreateNtuple("CEPC","Hit_Data");
    aM->CreateNtupleIColumn("SiPMcount",fEventAction->GetRofnumVec());//0
    aM->CreateNtupleIColumn("SiPMTowerIEta",fEventAction->GetTowerIEtas());
    aM->CreateNtupleIColumn("SiPMTowerIPhi",fEventAction->GetTowerIPhis());
    aM->CreateNtupleIColumn("SiPMID",fEventAction->GetRofPMTIDVec());//1

    for (int bin = 0; bin < fWavBin+1; bin++) { aM->CreateNtupleIColumn("SiPM"+std::to_string(900-bin*600/fWavBin)+"nm",fEventAction->GetWavelenCount(bin)); }
    aM->CreateNtupleIColumn("SiPMUV",fEventAction->GetWavelenCount(fWavBin+1));
    for (int bin = 0; bin < fTimeBin+1; bin++) { aM->CreateNtupleIColumn("SiPM"+std::to_string(100+bin*600/fTimeBin)+"hps",fEventAction->GetTimeCount(bin)); }
    aM->CreateNtupleIColumn("SiPMlate",fEventAction->GetTimeCount(fTimeBin+1));

    aM->CreateNtupleFColumn("Edep",fEventAction->GetEdeps());//3
    aM->CreateNtupleFColumn("EdepEle",fEventAction->GetEdepEles());
    aM->CreateNtupleFColumn("EdepGamma",fEventAction->GetEdepGammas());
    aM->CreateNtupleFColumn("EdepCharged",fEventAction->GetEdepChargeds());
    aM->CreateNtupleIColumn("EdepTowerIEta",fEventAction->GetEdepTowerIEtas());
    aM->CreateNtupleIColumn("EdepTowerIPhi",fEventAction->GetEdepTowerIPhis());

    aM->CreateNtupleFColumn("Leakage",fEventAction->GetRofleakVec_());//14
    aM->CreateNtupleIColumn("LeakID",fEventAction->GetRofleakIDVec_());//15
    aM->CreateNtupleFColumn("LeakageTotal",fEventAction->GetRofleaktotalVec_());//15

    aM->FinishNtuple();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WGR16RunAction::~WGR16RunAction()
{
  delete G4AnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WGR16RunAction::BeginOfRunAction(const G4Run*) {
  G4AnalysisManager* aM = G4AnalysisManager::Instance();

  // The default file name is set in WGR16RunAction::WGR16RunAction(),
  // it can be overwritten in a macro
  aM->OpenFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WGR16RunAction::EndOfRunAction(const G4Run*) {
  // save histograms & ntuple

  G4AnalysisManager* aM = G4AnalysisManager::Instance();
  aM->Write();
  aM->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
