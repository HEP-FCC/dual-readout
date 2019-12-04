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

#include "HepMCG4Interface.hh"

#include "G4RunManager.hh"
#include "G4LorentzVector.hh"
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4TransportationManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
HepMCG4Interface::HepMCG4Interface()
  : hepmcEvent(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
HepMCG4Interface::~HepMCG4Interface() {
  delete hepmcEvent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool HepMCG4Interface::CheckVertexInsideWorld(const G4ThreeVector& pos) const {
  G4Navigator* navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

  G4VPhysicalVolume* world = navigator->GetWorldVolume();
  G4VSolid* solid = world->GetLogicalVolume()->GetSolid();
  EInside qinside = solid->Inside(pos);

  if( qinside != kInside ) return false;
  else return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void HepMCG4Interface::HepMC2G4(const HepMC3::GenEvent* hepmcevt, G4Event* g4event) {
  for( auto vitr = hepmcevt->vertices().begin(); vitr != hepmcevt->vertices().end(); ++vitr ) { // loop for vertex ...
    // real vertex?
    G4bool qvtx=false;
    for (auto pitr = (*vitr)->particles_out().begin(); pitr != (*vitr)->particles_out().end(); ++pitr) {
      if (!(*pitr)->end_vertex() && (*pitr)->status()==1) {
        qvtx=true;
        break;
      }
    }
    if (!qvtx) continue;

    // check world boundary
    HepMC3::FourVector pos = (*vitr)->position();
    G4LorentzVector xvtx(pos.x(), pos.y(), pos.z(), pos.t());
    if (!CheckVertexInsideWorld(xvtx.vect()*mm)) continue;

    // create G4PrimaryVertex and associated G4PrimaryParticles
    G4PrimaryVertex* g4vtx = new G4PrimaryVertex(xvtx.x()*mm, xvtx.y()*mm, xvtx.z()*mm, xvtx.t()*mm/c_light);

    for (auto vpitr = (*vitr)->particles_out().begin(); vpitr != (*vitr)->particles_out().end(); ++vpitr) {
      if( (*vpitr)->status() != 1 ) continue;

      G4int pdgcode = (*vpitr)->pdg_id();
      pos = (*vpitr)->momentum();
      G4LorentzVector p(pos.px(), pos.py(), pos.pz(), pos.e());
      G4PrimaryParticle* g4prim = new G4PrimaryParticle(pdgcode, p.x()*MeV, p.y()*MeV, p.z()*MeV);

      g4vtx->SetPrimary(g4prim);
    }
    g4event->AddPrimaryVertex(g4vtx);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
HepMC3::GenEvent* HepMCG4Interface::GenerateHepMCEvent()
{
  HepMC3::GenEvent* aevent = new HepMC3::GenEvent();
  return aevent;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void HepMCG4Interface::GeneratePrimaryVertex(G4Event* anEvent)
{
  // delete previous event object
  delete hepmcEvent;

  // generate next event
  hepmcEvent = GenerateHepMCEvent();
  if(!hepmcEvent) {
    G4cout << "HepMCInterface: no generated particles. run terminated..."
           << G4endl;
    G4RunManager::GetRunManager()->AbortRun();
    return;
  }
  HepMC2G4(hepmcEvent, anEvent);
}
