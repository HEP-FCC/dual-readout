#include "HepMCG4Reader.hh"
#include "G4GenericMessenger.hh"

#include <iostream>
#include <fstream>

HepMCG4Reader::HepMCG4Reader(G4int seed, G4String hepMCpath)
: verbose(1), fMessenger(0), fSeed(seed), fHepMCpath(hepMCpath)
{
  DefineCommands();
  Initialize();
}

HepMCG4Reader::~HepMCG4Reader() {
  reader->close();
  delete reader;
  delete fMessenger;
}

void HepMCG4Reader::Initialize() {
  fHepMCpath += "_"+std::to_string(fSeed)+".root";
  reader = new HepMC3::ReaderRootTree(fHepMCpath.c_str());
}

HepMC3::GenEvent* HepMCG4Reader::GenerateHepMCEvent() {
  HepMC3::GenEvent* evt = new HepMC3::GenEvent(HepMC3::Units::GEV,HepMC3::Units::MM);
  reader->read_event(*evt);
  if( reader->failed() ) return 0;
  if( verbose>0 ) HepMC3::Print::listing(*evt);;

  return evt;
}

void HepMCG4Reader::DefineCommands() {
  fMessenger = new G4GenericMessenger(this, "/DRsim/hepMC/", "HepMC IO control");

  G4GenericMessenger::Command& verboseCmd = fMessenger->DeclareMethod("verbose",&HepMCG4Reader::SetVerboseLevel,"verbose level");
  verboseCmd.SetParameterName("verbose",true);
  verboseCmd.SetDefaultValue("1");
}
