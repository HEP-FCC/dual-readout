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

#ifndef WGR16PMTSD_h
#define WGR16PMTSD_h 1

#include "G4VSensitiveDetector.hh"
#include "WGR16PMTHit.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class WGR16PMTSD : public G4VSensitiveDetector {
public:
  WGR16PMTSD(const G4String& name, const G4String& hitsCollectionName);
  virtual ~WGR16PMTSD();

  virtual void Initialize(G4HCofThisEvent* HCE);
  virtual G4bool ProcessHits(G4Step* aStep,G4TouchableHistory*);
  virtual void EndOfEvent(G4HCofThisEvent* HCE);

private:
  WGR16PMTHitsCollection* fHitCollection;
  G4int fHCID;
  G4int fSDID;
  bool fIsR;
  bool fIsB;
  G4int fWavBin;
  G4int fTimeBin;

  G4double wavToE(G4double wav) { return h_Planck*c_light/wav; }

  G4int findBin(G4double en) {
    for (int i = 0; i < fWavBin+1; i++) {
      if ( en < wavToE( 900*nm - i*600*nm/(double)fWavBin ) ) return i;
      else continue;
    }
    return fWavBin+1;
  }

  G4int findTimeBin(G4double stepTime) {
    for (int i = 0; i < fTimeBin+1; i++) {
      if ( stepTime < ( 10*ns + i*60*ns/(double)fTimeBin ) ) return i;
      else continue;
    }
    return fTimeBin+1;
  }
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
