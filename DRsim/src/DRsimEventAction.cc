#include "DRsimEventAction.hh"
#include "DRsimRunAction.hh"
#include "DRsimPrimaryGeneratorAction.hh"
#include "DRsimDetectorConstruction.hh"

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
}

DRsimEventAction::~DRsimEventAction() {}

void DRsimEventAction::BeginOfEventAction(const G4Event*) {
	clear();

  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
  for (int i = 0; i < DRsimDetectorConstruction::sNumBarrel; i++) {
    fSiPMCollID.push_back(sdManager->GetCollectionID("BRC"+std::to_string(i)));
    fSiPMCollID.push_back(sdManager->GetCollectionID("BLC"+std::to_string(i)));
  }
  for (int i = 0; i < DRsimDetectorConstruction::sNumEndcap; i++) {
    fSiPMCollID.push_back(sdManager->GetCollectionID("ERC"+std::to_string(i)));
    fSiPMCollID.push_back(sdManager->GetCollectionID("ELC"+std::to_string(i)));
  }

  fEventData = new DRsimInterface::DRsimEventData();
}

void DRsimEventAction::clear() {
  fSiPMCollID.clear();
  fTowerMap.clear();
  fEdepMap.clear();
}

void DRsimEventAction::EndOfEventAction(const G4Event* event) {
  G4HCofThisEvent* hce = event->GetHCofThisEvent();
  if (!hce) {
    G4ExceptionDescription msg;
    msg << "No hits collection of this event found." << G4endl;
    G4Exception("DRsimEventAction::EndOfEventAction()",
    "DRsimCode001", JustWarning, msg);
    return;
  }

  G4int totSDNum = hce->GetNumberOfCollections();

  for (int iSD = 0; iSD < totSDNum; iSD++) {
    DRsimSiPMHitsCollection* sipmHC = 0;

    if (hce) {
      if(fSiPMCollID[iSD]>=0) sipmHC = (DRsimSiPMHitsCollection*)(hce->GetHC(fSiPMCollID[iSD]));
    }

    if (sipmHC) {
      G4int SiPMs = sipmHC->entries();
      for (G4int iHC = 0; iHC < SiPMs; iHC++) {
        fillHits((*sipmHC)[iHC]);
      }
    }
  }

  for (const auto& towerMap : fTowerMap) {
    fEventData->towers.push_back(towerMap.second);
  }

  for (const auto& edepMap : fEdepMap) {
    fEventData->Edeps.push_back(edepMap.second);
  }

  for (int iVtx = 0; iVtx < event->GetNumberOfPrimaryVertex(); iVtx++) {
    G4PrimaryVertex* vtx = event->GetPrimaryVertex(iVtx);

    for (int iPtc = 0; iPtc < vtx->GetNumberOfParticle(); iPtc++) {
      G4PrimaryParticle* ptc = vtx->GetPrimary(iPtc);
      fillPtcs(vtx,ptc);
    }
  }

  fEventData->event_number = DRsimPrimaryGeneratorAction::sIdxEvt;

  queue();

  delete fEventData;
}

void DRsimEventAction::fillHits(DRsimSiPMHit* hit) {
  DRsimInterface::DRsimSiPMData sipmData;
  sipmData.count = hit->GetPhotonCount();
  sipmData.SiPMnum = hit->GetSiPMnum();
  sipmData.x = hit->GetSiPMXY().first;
  sipmData.y = hit->GetSiPMXY().second;
  sipmData.pos = std::make_tuple(hit->GetSiPMpos().x(),hit->GetSiPMpos().y(),hit->GetSiPMpos().z());
  sipmData.timeStruct = hit->GetTimeStruct();
  sipmData.wavlenSpectrum = hit->GetWavlenSpectrum();

  toweriTiP towerTP = std::make_pair(hit->GetTowerTheta().first,hit->GetTowerPhi().first);
  auto towerIter = fTowerMap.find(towerTP);

  if ( towerIter==fTowerMap.end() ) {
    DRsimInterface::DRsimTowerData towerData;
    towerData.towerTheta = hit->GetTowerTheta();
    towerData.towerPhi = hit->GetTowerPhi();
    towerData.numx = hit->GetTowerXY().first;
    towerData.numy = hit->GetTowerXY().second;
    towerData.innerR = hit->GetTowerInnerR();
    towerData.towerH = hit->GetTowerH();
    towerData.dTheta = hit->GetTowerDTheta();
    towerData.SiPMs.push_back(sipmData);

    fTowerMap.insert(std::make_pair(towerTP,towerData));
  } else {
    towerIter->second.SiPMs.push_back(sipmData);
  }
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

void DRsimEventAction::fillEdeps(DRsimInterface::DRsimEdepData edepData) {
  toweriTiP towerTP = std::make_pair(edepData.iTheta,edepData.iPhi);
  auto towerIter = fEdepMap.find(towerTP);

  if ( towerIter==fEdepMap.end() ) {
    fEdepMap.insert(std::make_pair(towerTP,edepData));
  } else {
    towerIter->second.Edep += edepData.Edep;
    towerIter->second.EdepEle += edepData.EdepEle;
    towerIter->second.EdepGamma += edepData.EdepGamma;
    towerIter->second.EdepCharged += edepData.EdepCharged;
  }
}

void DRsimEventAction::fillLeaks(DRsimInterface::DRsimLeakageData leakData) {
  fEventData->leaks.push_back(leakData);
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
