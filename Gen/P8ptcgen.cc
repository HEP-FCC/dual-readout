#include "Pythia8/Pythia.h"

#include "HepMC3/GenEvent.h"
#include "HepMC3/Print.h"
#include "HepMC3/WriterRootTree.h"
#include "Pythia8ToHepMC3.h"

#include "fastjet/ClusterSequence.hh"
#include "fastjet/PseudoJet.hh"

#include "P8ptcgun.h"
#include "fastjetInterface.h"

#include <iostream>

using namespace Pythia8;

int main(int argc, char* argv[]) {

  // Check that correct number of command-line arguments
  if (argc != 4) {
    cerr << " Unexpected number of command-line arguments. \n You are"
         << " expected to provide one input and one output file name. \n"
         << " Program stopped! " << endl;
    return 1;
  }

  // Check that the provided input name corresponds to an existing file.
  ifstream is(argv[1]);
  if (!is) {
    cerr << " Command-line file " << argv[1] << " was not found. \n"
         << " Program stopped! " << endl;
    return 1;
  }

  // Interface for conversion from Pythia8::Event to HepMC event.
  HepMC3::Pythia8ToHepMC3 ToHepMC;

  // Specify file where HepMC events will be stored.
  std::string seed = argv[2];
  std::string filename = argv[3];
  HepMC3::WriterRootTree rootOutput(filename+"_"+seed+".root");
  fastjetInterface fjInterface;

  // Generator.
  Pythia pythia;
  Event& event      = pythia.event;
  ParticleData& pdt = pythia.particleData;

  // Read in commands from external file.
  pythia.readFile(argv[1]);
  pythia.readString("Random:seed = "+seed);

  // Extract settings to be used in the main program.
  int    nEvent    = pythia.mode("Main:numberOfEvents");
  int    nAbort    = pythia.mode("Main:timesAllowErrors");
  // Set typical energy per parton.
  int    idGun     = pythia.mode("Main:spareMode1");
  double eeGun     = pythia.parm("Main:spareParm1");
  bool   atRest    = pythia.flag("Main:spareFlag1");
  bool colSinglet  = pythia.flag("Main:spareFlag2");
  double theta     = pythia.parm("Main:spareParm2");
  double scale     = pythia.parm("Main:spareParm3");

  // Initialization.
  pythia.init();

  fjInterface.init(rootOutput.m_tree,"GenJets");
  P8ptcgun ptcgun(idGun, eeGun, theta, 0.);

  // FastJet
  std::vector<fastjet::PseudoJet> fjInputs;

  // Begin event loop.
  int iAbort = 0;
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    if (colSinglet) ptcgun.fillResonance( event, pdt, pythia.rndm, atRest );
    else {
      ptcgun.fillParton( event, pdt, pythia.rndm, atRest, scale );
      pythia.forceTimeShower( 1, 2, scale );
    }

    // Generate event.
    if (!pythia.next()) {

      // If failure because reached end of file then exit event loop.
      if (pythia.info.atEndOfFile()) {
        cout << " Aborted since reached end of Les Houches Event File\n";
        break;
      }

      // First few failures write off as "acceptable" errors, then quit.
      if (++iAbort < nAbort) continue;
      cout << " Event generation aborted prematurely, owing to error!\n";
      break;
    }

    // Construct new empty HepMC event and fill it.
    // Units will be as chosen for HepMC build, but can be changed
    // by arguments, e.g. GenEvt( HepMC::Units::GEV, HepMC::Units::MM)
    HepMC3::GenEvent* hepmcevt = new HepMC3::GenEvent(HepMC3::Units::GEV, HepMC3::Units::MM);
    ToHepMC.fill_next_event( pythia, hepmcevt );

    fjInputs.resize(0);

    // Loop over event record to decide what to pass to FastJet
    for (int i = 0; i < pythia.event.size(); ++i) {
      // Final state only
      if (!pythia.event[i].isFinal())        continue;

      // No neutrinos
      if (pythia.event[i].idAbs() == 12 || pythia.event[i].idAbs() == 14 ||
          pythia.event[i].idAbs() == 16)     continue;

      // Store as input to Fastjet
      fjInputs.push_back( fastjet::PseudoJet( pythia.event[i].px(),
        pythia.event[i].py(), pythia.event[i].pz(), pythia.event[i].e() ) );
    }

    if (fjInputs.size() == 0) {
      cout << "Error: event with no final state particles" << endl;
      continue;
    }

    // Run Fastjet algorithm
    fjInterface.runFastjet(fjInputs);

    rootOutput.write_event(*hepmcevt);
    delete hepmcevt;

    // List first few events.
    if (iEvent < 1) {
      event.list(true);
      // Also list junctions.
      event.listJunctions();
    }
  // End of event loop. Statistics.
  }
  rootOutput.close();
  pythia.stat();

  // Done.
  return 0;
}
