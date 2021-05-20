#include "SimG4SaveMCParticles.h"

// Geant4
#include "G4Event.hh"

// DD4hep
#include "DDG4/Geant4Hits.h"

#include "G4SystemOfUnits.hh"
#include "DD4hep/DD4hepUnits.h"

#include <stdexcept>
#include <functional>

SimG4SaveMCParticles::SimG4SaveMCParticles(podio::EventStore* store, podio::ROOTWriter* writer)
: pStore(store), pWriter(writer) {
  initialize();
}

SimG4SaveMCParticles::~SimG4SaveMCParticles() {}

void SimG4SaveMCParticles::initialize() {
  auto& MCparticles = pStore->create<edm4hep::MCParticleCollection>("MCParticles");
  mMCparticles = &MCparticles;
  pWriter->registerForWrite("MCParticles");

  return;
}

void SimG4SaveMCParticles::saveOutput(const G4Event* aEvent) const {
  for (int iVtx = 0; iVtx < aEvent->GetNumberOfPrimaryVertex(); iVtx++) {
    G4PrimaryVertex* vtx = aEvent->GetPrimaryVertex(iVtx);

    for (int iPtc = 0; iPtc < vtx->GetNumberOfParticle(); iPtc++) {
      G4PrimaryParticle* ptc = vtx->GetPrimary(iPtc);

      auto mcparticle = mMCparticles->create();
      mcparticle.setPDG(ptc->GetPDGcode());
      mcparticle.setGeneratorStatus(1); // primary particles naturally belong to final states
      mcparticle.setCharge(ptc->GetCharge());
      mcparticle.setMass( ptc->GetMass()*CLHEP::MeV/CLHEP::GeV );
      mcparticle.setMomentum( { static_cast<float>(ptc->GetPx()*CLHEP::MeV/CLHEP::GeV),
                                static_cast<float>(ptc->GetPy()*CLHEP::MeV/CLHEP::GeV),
                                static_cast<float>(ptc->GetPz()*CLHEP::MeV/CLHEP::GeV)} );
      mcparticle.setVertex( { vtx->GetX0()*CLHEP::millimeter, vtx->GetY0()*CLHEP::millimeter, vtx->GetZ0()*CLHEP::millimeter } );
    }
  }

  return;
}
