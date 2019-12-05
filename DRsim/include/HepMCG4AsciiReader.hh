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

#ifndef HEPMC_G4_ASCII_READER_H
#define HEPMC_G4_ASCII_READER_H

#include "HepMCG4Interface.hh"
#include "HepMC3/ReaderAsciiHepMC2.h"
#include "HepMC3/Units.h"
#include "HepMC3/Print.h"

class G4GenericMessenger;

class HepMCG4AsciiReader : public HepMCG4Interface {
protected:
  HepMC3::ReaderAsciiHepMC2* asciiInput;

  G4int verbose;

  virtual HepMC3::GenEvent* GenerateHepMCEvent();

public:
  HepMCG4AsciiReader(G4int seed, G4String hepMCpath);
  ~HepMCG4AsciiReader();

  // set/get methods
  void SetVerboseLevel(G4int i) { verbose = i; }
  G4int GetVerboseLevel() const { return verbose; }

  // methods...
  void Initialize();

private:
  void DefineCommands();

  G4GenericMessenger* fMessenger;
  G4int fSeed;
  G4String fHepMCpath;
};

#endif
