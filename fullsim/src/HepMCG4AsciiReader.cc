///////////////////////////////////////////////////////////////////////
//// <CEPC>                                                        ////
//// Wedge Geometry for Dual-reaout calorimter                     ////
////                                                               ////
//// Original Author: Sanghyun Ko, Seoul National University       ////
////                                                               ////
//// E-Mail: sang.hyun.ko@cern.ch                                  ////
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
//

#include "HepMCG4AsciiReader.hh"
#include "G4GenericMessenger.hh"

#include <iostream>
#include <fstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
HepMCG4AsciiReader::HepMCG4AsciiReader(G4int seed, G4String hepMCpath)
: verbose(0), fMessenger(0), fSeed(seed), fHepMCpath(hepMCpath)
{
  DefineCommands();
  Initialize();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
HepMCG4AsciiReader::~HepMCG4AsciiReader() {
  delete asciiInput;
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void HepMCG4AsciiReader::Initialize() {
  fHepMCpath += "_"+std::to_string(fSeed)+".dat";
  asciiInput = new HepMC3::ReaderAsciiHepMC2(fHepMCpath.c_str());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
HepMC3::GenEvent* HepMCG4AsciiReader::GenerateHepMCEvent() {
  HepMC3::GenEvent* evt = new HepMC3::GenEvent(HepMC3::Units::MEV,HepMC3::Units::MM);
  asciiInput->read_event(*evt);
  if( asciiInput->failed() ) return 0;
  if( verbose>0 ) HepMC3::Print::listing(*evt);;

  return evt;
}

void HepMCG4AsciiReader::DefineCommands() {
  fMessenger = new G4GenericMessenger(this, "/WGR16/hepMC/", "HepMC IO control");

  G4GenericMessenger::Command& verboseCmd = fMessenger->DeclareMethod("verbose",&HepMCG4AsciiReader::SetVerboseLevel,"verbose level");
  verboseCmd.SetParameterName("verbose",true);
  verboseCmd.SetDefaultValue("0");
}
