#include "HepMCG4AsciiReader.hh"
#include "G4GenericMessenger.hh"

#include <iostream>
#include <fstream>

HepMCG4AsciiReader::HepMCG4AsciiReader(G4int seed, G4String hepMCpath)
: verbose(0), fMessenger(0), fSeed(seed), fHepMCpath(hepMCpath)
{
  DefineCommands();
  Initialize();
}

HepMCG4AsciiReader::~HepMCG4AsciiReader() {
  delete asciiInput;
  delete fMessenger;
}

void HepMCG4AsciiReader::Initialize() {
  fHepMCpath += "_"+std::to_string(fSeed)+".dat";
  asciiInput = new HepMC3::ReaderAsciiHepMC2(fHepMCpath.c_str());
}

HepMC3::GenEvent* HepMCG4AsciiReader::GenerateHepMCEvent() {
  HepMC3::GenEvent* evt = new HepMC3::GenEvent(HepMC3::Units::MEV,HepMC3::Units::MM);
  asciiInput->read_event(*evt);
  if( asciiInput->failed() ) return 0;
  if( verbose>0 ) HepMC3::Print::listing(*evt);;

  return evt;
}

void HepMCG4AsciiReader::DefineCommands() {
  fMessenger = new G4GenericMessenger(this, "/DRsim/hepMC/", "HepMC IO control");

  G4GenericMessenger::Command& verboseCmd = fMessenger->DeclareMethod("verbose",&HepMCG4AsciiReader::SetVerboseLevel,"verbose level");
  verboseCmd.SetParameterName("verbose",true);
  verboseCmd.SetDefaultValue("0");
}
