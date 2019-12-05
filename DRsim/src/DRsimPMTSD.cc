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

#include "WGR16PMTSD.hh"
#include "WGR16PMTHit.hh"

#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//WGR16PMTSD::WGR16PMTSD(G4String name)
WGR16PMTSD::WGR16PMTSD(const G4String& name, const G4String& hitsCollectionName)
: G4VSensitiveDetector(name), fHitCollection(0), fHCID(-1), fWavBin(60), fTimeBin(300)
{
  fSDID = std::stoi(name.substr(2));
  fIsR = (name.find("R")==std::string::npos) ? false : true;
  fIsB = (name.find("B")==std::string::npos) ? false : true;
  collectionName.insert(hitsCollectionName);
  if(!fIsB) fSDID += 52;
  if(!fIsR) fSDID = -fSDID -1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WGR16PMTSD::~WGR16PMTSD() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WGR16PMTSD::Initialize(G4HCofThisEvent* hce) {
  fHitCollection = new WGR16PMTHitsCollection(SensitiveDetectorName,collectionName[0]);
  if (fHCID<0) { fHCID = GetCollectionID(0); }
  hce->AddHitsCollection(fHCID,fHitCollection);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool WGR16PMTSD::ProcessHits(G4Step* step, G4TouchableHistory*) {
  if(step->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false;

  G4int PMTnum = step->GetPostStepPoint()->GetTouchable()->GetVolume(1)->GetCopyNo();
  G4int towerPhi = step->GetPostStepPoint()->GetTouchable()->GetVolume(3)->GetCopyNo();
  G4int nofHits = fHitCollection->entries();
  G4double hitTime = step->GetPostStepPoint()->GetGlobalTime();
  G4double energy = step->GetTrack()->GetTotalEnergy();
  WGR16PMTHit* hit = NULL;

  for (G4int i = 0; i < nofHits; i++) {
    if ( (*fHitCollection)[i]->GetPMTnum()==PMTnum && (*fHitCollection)[i]->GetTowerIEta()==fSDID && (*fHitCollection)[i]->GetTowerIPhi()==towerPhi) {
      hit = (*fHitCollection)[i];
      break;
    }
  }

  if (hit==NULL) {
    hit = new WGR16PMTHit(fWavBin,fTimeBin);
    hit->SetPMTnum(PMTnum);
    hit->SetTowerIEta(fSDID);
    hit->SetTowerIPhi(towerPhi);
    fHitCollection->insert(hit);
  }

  G4int bin = findBin(energy);
  hit->photonCount();
  hit->wavelenCount(bin);

  G4int timeBin = findTimeBin(hitTime);
  hit->timeCount(timeBin);

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WGR16PMTSD::EndOfEvent(G4HCofThisEvent*) {
  if ( verboseLevel>1 ) {
    G4int nofHits = fHitCollection->entries();
    G4cout
    << G4endl
    << "-------->Hits Collection: in this event they are " << nofHits
    << " hits in the tracker chambers: " << G4endl;
    for ( G4int i=0; i<nofHits; i++ ) (*fHitCollection)[i]->Print();
  }
}
