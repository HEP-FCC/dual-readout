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
: G4UserEventAction(), pSaveHits(nullptr), pSaveMCParticles(nullptr), pStore(nullptr), pWriter(nullptr), mEvtHeaders(nullptr)
{
  // set printing per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);
}

DRsimEventAction::~DRsimEventAction() {}

void DRsimEventAction::initialize() {
  *mEvtHeaders = pStore->create<edm4hep::EventHeaderCollection>("EventHeader");
  pWriter->registerForWrite("EventHeader");

  return;
}

void DRsimEventAction::BeginOfEventAction(const G4Event*) {
  if (pSaveHits==nullptr)
    G4Exception("DRsimEventAction::BeginOfEventAction()","",FatalException,
                "Should initialize SimG4SaveDRcaloHits before starting the Run");
  if (pSaveMCParticles==nullptr)
    G4Exception("DRsimEventAction::BeginOfEventAction()","",FatalException,
                "Should initialize SimG4SaveMCParticles before starting the Run");
  if ( pStore==nullptr || pWriter==nullptr )
    G4Exception("DRsimEventAction::BeginOfEventAction()","",FatalException,
                "Should initialize PODIO Writer & Eventstore before starting the Run");

	clear();

  return;
}

void DRsimEventAction::EndOfEventAction(const G4Event* event) {
  pSaveHits->saveOutput(event);
  pSaveMCParticles->saveOutput(event);

  auto evtHeader = mEvtHeaders->create();
  evtHeader.setEventNumber(DRsimPrimaryGeneratorAction::sIdxEvt);
  evtHeader.setRunNumber(mRunNumber);

  writeEvent();
  clearCollections();
}
