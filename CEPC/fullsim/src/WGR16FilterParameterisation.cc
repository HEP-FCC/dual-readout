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
//
// $Id: FilterParameterisation.cc 76474 2013-11-11 10:36:34Z gcosmo $
//
/// \file FilterParameterisation.cc
/// \brief Implementation of the FilterParameterisation class

#include "WGR16FilterParameterisation.hh"

#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WGR16FilterParameterisation::WGR16FilterParameterisation(const G4int numx, const G4int numy)
: G4VPVParameterisation()
{
  for (G4int copyNo=0;copyNo<numx*numy;copyNo++) {
    G4int column = copyNo / numy;
    G4int row = copyNo % numy;

    if ( !IsCerenkov(column,row) ) {
      fXFilter.push_back( (column-numx/2)*1.5*mm + ( numx%2==0 ? 0.75*mm : 0 ) );
      fYFilter.push_back( (row-numy/2)*1.5*mm + ( numy%2==0 ? 0.75*mm : 0 ) );
    }
  }
  fNumx = numx;
  fNumy = numy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WGR16FilterParameterisation::~WGR16FilterParameterisation()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WGR16FilterParameterisation::ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* physVol) const {
  physVol->SetTranslation(G4ThreeVector(fXFilter[copyNo],fYFilter[copyNo],0.));
}

bool WGR16FilterParameterisation::IsCerenkov(G4int col, G4int row) const {
  bool isCeren = false;
  if ( col%2 == 1 ) { isCeren = !isCeren; }
  if ( row%2 == 1 ) { isCeren = !isCeren; }
  return isCeren;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
