#include "DRsimEventAction.hh"
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
: G4UserEventAction(), pSaveHits(nullptr), pStore(nullptr), pWriter(nullptr)
{
  // set printing per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);
}

DRsimEventAction::~DRsimEventAction() {}

void DRsimEventAction::BeginOfEventAction(const G4Event*) {
  if (pSaveHits==nullptr)
    G4Exception("DRsimEventAction::BeginOfEventAction()","",FatalException,
                "Should initialize SimG4SaveDRcaloHits before starting the Run");
  if ( pStore==nullptr || pWriter==nullptr )
    G4Exception("DRsimEventAction::BeginOfEventAction()","",FatalException,
                "Should initialize PODIO Writer & Eventstore before starting the Run");

	clear();
}

void DRsimEventAction::clear() {
  fEventData->clear();
}

void DRsimEventAction::EndOfEventAction(const G4Event* event) {
  pSaveHits->saveOutput(event);

  for (int iVtx = 0; iVtx < event->GetNumberOfPrimaryVertex(); iVtx++) {
    G4PrimaryVertex* vtx = event->GetPrimaryVertex(iVtx);

    for (int iPtc = 0; iPtc < vtx->GetNumberOfParticle(); iPtc++) {
      G4PrimaryParticle* ptc = vtx->GetPrimary(iPtc);
      fillPtcs(vtx,ptc);
    }
  }

  fEventData->event_number = DRsimPrimaryGeneratorAction::sIdxEvt;

  writeEvent();
  clearCollections();

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
  // while ( DRsimRunAction::sNumEvt != DRsimPrimaryGeneratorAction::sIdxEvt ) {
  //   G4AutoLock lock(&DRsimEventActionMutex);
  //   if ( DRsimRunAction::sNumEvt == DRsimPrimaryGeneratorAction::sIdxEvt ) break;
  //   G4CONDITIONWAIT(&DRsimEventActionCV, &lock);
  // }
  // G4AutoLock lock(&DRsimEventActionMutex);
  // DRsimRunAction::sRootIO->fill(fEventData);
  // DRsimRunAction::sNumEvt++;
  // G4CONDITIONBROADCAST(&DRsimEventActionCV);
}
