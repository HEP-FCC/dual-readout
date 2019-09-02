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

#include "WGR16EventAction.hh"
#include "WGR16PMTHit.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4Track.hh"
#include <string.h>
#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
using namespace std;
WGR16EventAction::WGR16EventAction(G4int wavBin, G4int timeBin)
: G4UserEventAction(),
 fPMTID(0),fHitCount(0),fTowerIEta(0),fTowerIPhi(0),fWavelenCount(wavBin+2),fTimeCount(timeBin+2),
 fEdep(0),fEdepEle(0),fEdepGamma(0),fEdepCharged(0)
{
  // set printing per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);
  fWavBin = wavBin;
  fTimeBin = timeBin;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WGR16EventAction::~WGR16EventAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WGR16EventAction::BeginOfEventAction(const G4Event*) {
  G4cout << "Event action start" << G4endl;
	ClearVectors();

  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
  for (int i = 0; i < 52; i++) {
    fSiPMCollID.push_back(sdManager->GetCollectionID("BRC"+std::to_string(i)));
    fSiPMCollID.push_back(sdManager->GetCollectionID("BLC"+std::to_string(i)));
  }
  for (int i = 0; i < 40; i++) {
    fSiPMCollID.push_back(sdManager->GetCollectionID("ERC"+std::to_string(i)));
    fSiPMCollID.push_back(sdManager->GetCollectionID("ELC"+std::to_string(i)));
  }
}

void WGR16EventAction::ClearVectors() {
  fHitCount.clear();
  fPMTID.clear();
  fTowerIEta.clear();
  fTowerIPhi.clear();
  fSiPMCollID.clear();

  for (G4int bin = 0; bin < fWavBin+2; bin++) { fWavelenCount.at(bin).clear(); }
  for (G4int bin = 0; bin < fTimeBin+2; bin++) { fTimeCount.at(bin).clear(); }

  fEdep.clear();
  fEdepEle.clear();
  fEdepGamma.clear();
  fEdepCharged.clear();
  fEdepTowerIEta.clear();
  fEdepTowerIPhi.clear();
  fEdepIterMap.clear();

  fleakage.clear();
  fleakID.clear();
  fleakagetotal.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WGR16EventAction::EndOfEventAction(const G4Event* event) {
  G4cout << "start end of event action" << G4endl;
  hce = event->GetHCofThisEvent();
  if (!hce) {
    G4ExceptionDescription msg;
    msg << "No hits collection of this event found." << G4endl;
    G4Exception("WGR16EventAction::EndOfEventAction()",
    "WGR16Code001", JustWarning, msg);
    return;
  }

  G4AnalysisManager *aM = G4AnalysisManager::Instance();

  totSDNum = hce->GetNumberOfCollections();

  for (int j = 0; j < totSDNum; j++) {
    WGR16PMTHitsCollection* PHC = 0;

    if (hce) {
      if(fSiPMCollID[j]>=0) PHC = (WGR16PMTHitsCollection*)(hce->GetHC(fSiPMCollID[j]));
    }

    if (PHC) {
      G4int PMTs = PHC->entries();
      for (G4int i = 0; i < PMTs; i++) {
        fHitCount.push_back((*PHC)[i]->GetPhotonCount());
        fPMTID.push_back((*PHC)[i]->GetPMTnum());
        fTowerIEta.push_back((*PHC)[i]->GetTowerIEta());
        fTowerIPhi.push_back((*PHC)[i]->GetTowerIPhi());

        for (G4int bin = 0; bin < fWavBin+2; bin++) { fWavelenCount.at(bin).push_back( (*PHC)[i]->GetWavelenCount(bin) ); }
        for (G4int bin = 0; bin < fTimeBin+2; bin++) { fTimeCount.at(bin).push_back( (*PHC)[i]->GetTimeCount(bin) ); }
      }
    }
  }
  aM->AddNtupleRow(0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
