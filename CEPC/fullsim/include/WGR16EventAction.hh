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

using namespace std;

#ifndef WGR16EventAction_h
#define WGR16EventAction_h 1

#include "WGR16RunAction.hh"

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include "G4HCofThisEvent.hh"
#include "WGR16PMTHit.hh"
#include "G4VHitsCollection.hh"
#include "G4Track.hh"

class WGR16EventAction : public G4UserEventAction {
public:
  WGR16EventAction(G4int wavBin, G4int timeBin);
  virtual ~WGR16EventAction();

  void AddStepInfo(G4float edep, G4float edepEle, G4float edepGamma, G4float edepCharged, G4int towerIEta, G4int towerIPhi) {
    std::pair<int,int> towerEP (towerIEta,towerIPhi);
    std::map<std::pair<int,int>, unsigned int>::const_iterator citer = fEdepIterMap.find(towerEP);
    if (citer==fEdepIterMap.end()) {
      fEdepIterMap.insert( std::pair<std::pair<int,int>,unsigned int> (towerEP,(unsigned int)fEdep.size()) );
      fEdep.push_back(edep);
      fEdepEle.push_back(edepEle);
      fEdepGamma.push_back(edepGamma);
      fEdepCharged.push_back(edepCharged);
      fEdepTowerIEta.push_back(towerIEta);
      fEdepTowerIPhi.push_back(towerIPhi);
    } else {
      unsigned int where = citer->second;
      fEdep[where] += edep;
      fEdepEle[where] += edepEle;
      fEdepGamma[where] += edepGamma;
      fEdepCharged[where] += edepCharged;
    }
  }

  void AddLeakage(G4int ID, G4float leakage, G4float leaktotal){
    fleakID.push_back(ID);
    fleakage.push_back(leakage);
    fleakagetotal.push_back(leaktotal);
  }

  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);

  std::vector<G4int>& GetRofnumVec() { return fHitCount; }
  std::vector<G4int>& GetRofPMTIDVec() { return fPMTID; }
  std::vector<G4int>& GetTowerIEtas() { return fTowerIEta; }
  std::vector<G4int>& GetTowerIPhis() { return fTowerIPhi; }
  std::vector<G4int>& GetWavelenCount(G4int bin) { return fWavelenCount.at(bin); }
  std::vector<G4int>& GetTimeCount(G4int bin) { return fTimeCount.at(bin); }

  std::vector<G4float>& GetEdeps() { return fEdep; }
  std::vector<G4float>& GetEdepEles() { return fEdepEle; }
  std::vector<G4float>& GetEdepGammas() { return fEdepGamma; }
  std::vector<G4float>& GetEdepChargeds() { return fEdepCharged; }
  std::vector<G4int>& GetEdepTowerIEtas() { return fEdepTowerIEta; }
  std::vector<G4int>& GetEdepTowerIPhis() { return fEdepTowerIPhi; }

  std::vector<G4int>& GetRofleakIDVec_() { return fleakID; }
  std::vector<G4float>& GetRofleakVec_() { return fleakage; }
  std::vector<G4float>& GetRofleaktotalVec_() { return fleakagetotal; }

private:
  void ClearVectors();
  G4int flag;
  G4int flagnum;
  G4int flag_;
  G4int flagnum_;

  std::vector<G4int> fPMTID;
  std::vector<G4int> fHitCount;
  std::vector<G4int> fTowerIEta;
  std::vector<G4int> fTowerIPhi;
  std::vector<G4int> fSiPMCollID;
  std::vector<std::vector<G4int>> fWavelenCount;
  std::vector<std::vector<G4int>> fTimeCount;
  G4int fWavBin;
  G4int fTimeBin;

  std::vector<G4float> fEdep;
  std::vector<G4float> fEdepEle;
  std::vector<G4float> fEdepGamma;
  std::vector<G4float> fEdepCharged;
  std::vector<G4int> fEdepTowerIEta;
  std::vector<G4int> fEdepTowerIPhi;
  std::map<std::pair<int,int>, unsigned int> fEdepIterMap;

  vector<G4float> fleakage;
  vector<G4float> fleakagetotal;
  vector<G4int> fleakID;

  G4HCofThisEvent* hce;
  G4int totSDNum;
  G4int hitentries;
  WGR16PMTHitsCollection* HC;
  WGR16PMTHit* hit;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
