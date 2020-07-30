#include "DRsimEventAction.hh"
#include "DRsimRunAction.hh"
#include "DRsimPrimaryGeneratorAction.hh"

#include "G4PrimaryVertex.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4AutoLock.hh"
#include "G4Threading.hh"

namespace {
  G4Mutex DRsimEventActionMutex = G4MUTEX_INITIALIZER;
  G4Condition DRsimEventActionCV = G4CONDITION_INITIALIZER;
}

DRsimEventAction::DRsimEventAction()
: G4UserEventAction()
{
  // set printing per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);

  fSaveHits = new SimG4SaveDRcaloHits();
  fEventData = new DRsimInterface::DRsimEventData();
}

DRsimEventAction::~DRsimEventAction() {
  if (fSaveHits) delete fSaveHits;
  if (fEventData) delete fEventData;
}

void DRsimEventAction::BeginOfEventAction(const G4Event*) {
	clear();

  fSaveHits->setEventData(fEventData);
}

void DRsimEventAction::clear() {
  fEventData->clear();
}

void DRsimEventAction::EndOfEventAction(const G4Event* event) {
  fSaveHits->saveOutput(event);

  for (int iVtx = 0; iVtx < event->GetNumberOfPrimaryVertex(); iVtx++) {
    G4PrimaryVertex* vtx = event->GetPrimaryVertex(iVtx);

    for (int iPtc = 0; iPtc < vtx->GetNumberOfParticle(); iPtc++) {
      G4PrimaryParticle* ptc = vtx->GetPrimary(iPtc);
      fillPtcs(vtx,ptc);
    }
  }

  fEventData->event_number = DRsimPrimaryGeneratorAction::sIdxEvt;

  queue();
  clear();
}

void DRsimEventAction::fillPtcs(G4PrimaryVertex* vtx, G4PrimaryParticle* ptc) {
  DRsimInterface::DRsimGenData GenData;
  GenData.E = ptc->GetTotalEnergy();
  GenData.px = ptc->GetPx();
  GenData.py = ptc->GetPy();
  GenData.pz = ptc->GetPz();
  GenData.pdgId = ptc->GetPDGcode();
  GenData.vx = vtx->GetX0();
  GenData.vy = vtx->GetY0();
  GenData.vz = vtx->GetZ0();
  GenData.vt = vtx->GetT0();

  fEventData->GenPtcs.push_back(GenData);
}

void DRsimEventAction::queue() {
  while ( DRsimRunAction::sNumEvt != DRsimPrimaryGeneratorAction::sIdxEvt ) {
    G4AutoLock lock(&DRsimEventActionMutex);
    if ( DRsimRunAction::sNumEvt == DRsimPrimaryGeneratorAction::sIdxEvt ) break;
    G4CONDITIONWAIT(&DRsimEventActionCV, &lock);
  }
  G4AutoLock lock(&DRsimEventActionMutex);
  DRsimRunAction::sRootIO->fill(fEventData);
  DRsimRunAction::sNumEvt++;
  G4CONDITIONBROADCAST(&DRsimEventActionCV);
}
