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

#include "WGR16PMTHit.hh"

#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal G4Allocator<WGR16PMTHit>* WGR16PMTHitAllocator = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//WGR16PMTHit::WGR16PMTHit(G4int i,G4double t)
WGR16PMTHit::WGR16PMTHit(G4int wavBin, G4int timeBin)
: G4VHit(),
  fPMTnum(0),
  fPhotons(0),
  fTowerIEta(999),
  fTowerIPhi(999),
  fWavelen(wavBin+2,0),
  fTimeCount(timeBin+2,0),
  fWavBin(wavBin),
  fTimeBin(timeBin)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WGR16PMTHit::~WGR16PMTHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WGR16PMTHit::WGR16PMTHit(const WGR16PMTHit &right)
: G4VHit() {
  fPMTnum = right.fPMTnum;
  fPhotons = right.fPhotons;
  fTowerIEta = right.fTowerIEta;
  fTowerIPhi = right.fTowerIPhi;
  fWavelen = right.fWavelen;
  fTimeCount = right.fTimeCount;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const WGR16PMTHit& WGR16PMTHit::operator=(const WGR16PMTHit &right) {
  fPMTnum = right.fPMTnum;
  fPhotons = right.fPhotons;
  fTowerIEta = right.fTowerIEta;
  fTowerIPhi = right.fTowerIPhi;
  fWavelen = right.fWavelen;
  fTimeCount = right.fTimeCount;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int WGR16PMTHit::operator==(const WGR16PMTHit &right) const {
  return (fPMTnum==right.fPMTnum && fTowerIEta==right.fTowerIEta && fTowerIPhi==right.fTowerIPhi);
}

void* WGR16PMTHit::operator new(size_t) {
  if (!WGR16PMTHitAllocator) WGR16PMTHitAllocator = new G4Allocator<WGR16PMTHit>;
  return (void*)WGR16PMTHitAllocator->MallocSingle();
}

void WGR16PMTHit::operator delete(void*aHit) {
  WGR16PMTHitAllocator->FreeSingle((WGR16PMTHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WGR16PMTHit::Draw() {
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager) {}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WGR16PMTHit::Print() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
