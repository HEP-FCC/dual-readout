#include "DRsimEventAction.hh"
#include "DRsimPMTHit.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4Track.hh"
#include <string.h>
#include <vector>

using namespace std;
DRsimEventAction::DRsimEventAction(G4int wavBin, G4int timeBin)
: G4UserEventAction(),
 fPMTID(0),fHitCount(0),fTowerIEta(0),fTowerIPhi(0),fWavelenCount(wavBin+2),fTimeCount(timeBin+2),
 fEdep(0),fEdepEle(0),fEdepGamma(0),fEdepCharged(0)
{
  // set printing per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);
  fWavBin = wavBin;
  fTimeBin = timeBin;
}

DRsimEventAction::~DRsimEventAction() {}

void DRsimEventAction::BeginOfEventAction(const G4Event*) {
  G4cout << "Event action start" << G4endl;
	ClearVectors();

  G4SDManager* sdManager = G4SDManager::GetSDMpointer();
  for (int i = 0; i < 52; i++) {
    fSiPMCollID.push_back(sdManager->GetCollectionID("BRC"+std::to_string(i)));
    fSiPMCollID.push_back(sdManager->GetCollectionID("BLC"+std::to_string(i)));
  }
  for (int i = 0; i < 40; i++) {
    fSiPMCollID.push_back(sdManager->GetCollectionID("ERC"+std::to_string(i)));
    fSiPMCollID.push_back(sdManager->GetCollectionID("ELC"+std::to_string(i)));
  }
}

void DRsimEventAction::ClearVectors() {
  fHitCount.clear();
  fPMTID.clear();
  fTowerIEta.clear();
  fTowerIPhi.clear();
  fSiPMCollID.clear();

  for (G4int bin = 0; bin < fWavBin+2; bin++) { fWavelenCount.at(bin).clear(); }
  for (G4int bin = 0; bin < fTimeBin+2; bin++) { fTimeCount.at(bin).clear(); }

  fEdep.clear();
  fEdepEle.clear();
  fEdepGamma.clear();
  fEdepCharged.clear();
  fEdepTowerIEta.clear();
  fEdepTowerIPhi.clear();
  fEdepIterMap.clear();

  fleakage.clear();
  fleakID.clear();
  fleakagetotal.clear();
}

void DRsimEventAction::EndOfEventAction(const G4Event* event) {
  G4cout << "start end of event action" << G4endl;
  hce = event->GetHCofThisEvent();
  if (!hce) {
    G4ExceptionDescription msg;
    msg << "No hits collection of this event found." << G4endl;
    G4Exception("DRsimEventAction::EndOfEventAction()",
    "DRsimCode001", JustWarning, msg);
    return;
  }

  G4AnalysisManager *aM = G4AnalysisManager::Instance();

  totSDNum = hce->GetNumberOfCollections();

  for (int j = 0; j < totSDNum; j++) {
    DRsimPMTHitsCollection* PHC = 0;

    if (hce) {
      if(fSiPMCollID[j]>=0) PHC = (DRsimPMTHitsCollection*)(hce->GetHC(fSiPMCollID[j]));
    }

    if (PHC) {
      G4int PMTs = PHC->entries();
      for (G4int i = 0; i < PMTs; i++) {
        fHitCount.push_back((*PHC)[i]->GetPhotonCount());
        fPMTID.push_back((*PHC)[i]->GetPMTnum());
        fTowerIEta.push_back((*PHC)[i]->GetTowerIEta());
        fTowerIPhi.push_back((*PHC)[i]->GetTowerIPhi());

        for (G4int bin = 0; bin < fWavBin+2; bin++) { fWavelenCount.at(bin).push_back( (*PHC)[i]->GetWavelenCount(bin) ); }
        for (G4int bin = 0; bin < fTimeBin+2; bin++) { fTimeCount.at(bin).push_back( (*PHC)[i]->GetTimeCount(bin) ); }
      }
    }
  }
  aM->AddNtupleRow(0);
}
