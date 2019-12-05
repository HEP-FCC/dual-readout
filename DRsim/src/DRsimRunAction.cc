#include "DRsimDetectorConstruction.hh"
#include "DRsimRunAction.hh"
#include "DRsimEventAction.hh"
#include "DRsimPrimaryGeneratorAction.hh"
#include "G4Run.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"

#include <vector>
using namespace std;

DRsimRunAction::DRsimRunAction(DRsimEventAction* eventAction, int seed, G4int wavBin, G4int timeBin)
: G4UserRunAction(), fEventAction(eventAction)
{
  fSeed = seed;
  fWavBin = wavBin;
  fTimeBin = timeBin;
  G4AnalysisManager* aM = G4AnalysisManager::Instance();
  G4cout << "Using " << aM->GetType() << G4endl;

  char outputname[128]; std::sprintf(outputname,"dr_seednum%d",fSeed);

  // Default settings
  aM->SetVerboseLevel(1);
  aM->SetNtupleMerging(true);
  aM->SetFileName(outputname);

  // Creating ntuple

  if ( fEventAction ) {
    aM->CreateNtuple("CEPC","Hit_Data");
    aM->CreateNtupleIColumn("SiPMcount",fEventAction->GetRofnumVec());//0
    aM->CreateNtupleIColumn("SiPMTowerIEta",fEventAction->GetTowerIEtas());
    aM->CreateNtupleIColumn("SiPMTowerIPhi",fEventAction->GetTowerIPhis());
    aM->CreateNtupleIColumn("SiPMID",fEventAction->GetRofPMTIDVec());//1

    for (int bin = 0; bin < fWavBin+1; bin++) { aM->CreateNtupleIColumn("SiPM"+std::to_string(900-bin*600/fWavBin)+"nm",fEventAction->GetWavelenCount(bin)); }
    aM->CreateNtupleIColumn("SiPMUV",fEventAction->GetWavelenCount(fWavBin+1));
    for (int bin = 0; bin < fTimeBin+1; bin++) { aM->CreateNtupleIColumn("SiPM"+std::to_string(100+bin*600/fTimeBin)+"hps",fEventAction->GetTimeCount(bin)); }
    aM->CreateNtupleIColumn("SiPMlate",fEventAction->GetTimeCount(fTimeBin+1));

    aM->CreateNtupleFColumn("Edep",fEventAction->GetEdeps());//3
    aM->CreateNtupleFColumn("EdepEle",fEventAction->GetEdepEles());
    aM->CreateNtupleFColumn("EdepGamma",fEventAction->GetEdepGammas());
    aM->CreateNtupleFColumn("EdepCharged",fEventAction->GetEdepChargeds());
    aM->CreateNtupleIColumn("EdepTowerIEta",fEventAction->GetEdepTowerIEtas());
    aM->CreateNtupleIColumn("EdepTowerIPhi",fEventAction->GetEdepTowerIPhis());

    aM->CreateNtupleFColumn("Leakage",fEventAction->GetRofleakVec_());//14
    aM->CreateNtupleIColumn("LeakID",fEventAction->GetRofleakIDVec_());//15
    aM->CreateNtupleFColumn("LeakageTotal",fEventAction->GetRofleaktotalVec_());//15

    aM->FinishNtuple();
  }
}

DRsimRunAction::~DRsimRunAction()
{
  delete G4AnalysisManager::Instance();
}

void DRsimRunAction::BeginOfRunAction(const G4Run*) {
  G4AnalysisManager* aM = G4AnalysisManager::Instance();

  // The default file name is set in DRsimRunAction::DRsimRunAction(),
  // it can be overwritten in a macro
  aM->OpenFile();
}

void DRsimRunAction::EndOfRunAction(const G4Run*) {
  // save histograms & ntuple

  G4AnalysisManager* aM = G4AnalysisManager::Instance();
  aM->Write();
  aM->CloseFile();
}
