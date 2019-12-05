#ifndef DRsimEventAction_h
#define DRsimEventAction_h 1

using namespace std;

#include <vector>

#include "globals.hh"
#include "G4UserEventAction.hh"
#include "G4ThreeVector.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4Track.hh"

#include "DRsimPMTHit.hh"
#include "DRsimRunAction.hh"

class DRsimEventAction : public G4UserEventAction {
public:
  DRsimEventAction(G4int wavBin, G4int timeBin);
  virtual ~DRsimEventAction();

  void AddStepInfo(G4float edep, G4float edepEle, G4float edepGamma, G4float edepCharged, G4int towerIEta, G4int towerIPhi) {
    std::pair<int,int> towerEP (towerIEta,towerIPhi);
    std::map<std::pair<int,int>, unsigned int>::const_iterator citer = fEdepIterMap.find(towerEP);
    if (citer==fEdepIterMap.end()) {
      fEdepIterMap.insert( std::pair<std::pair<int,int>,unsigned int> (towerEP,(unsigned int)fEdep.size()) );
      fEdep.push_back(edep);
      fEdepEle.push_back(edepEle);
      fEdepGamma.push_back(edepGamma);
      fEdepCharged.push_back(edepCharged);
      fEdepTowerIEta.push_back(towerIEta);
      fEdepTowerIPhi.push_back(towerIPhi);
    } else {
      unsigned int where = citer->second;
      fEdep[where] += edep;
      fEdepEle[where] += edepEle;
      fEdepGamma[where] += edepGamma;
      fEdepCharged[where] += edepCharged;
    }
  }

  void AddLeakage(G4int ID, G4float leakage, G4float leaktotal){
    fleakID.push_back(ID);
    fleakage.push_back(leakage);
    fleakagetotal.push_back(leaktotal);
  }

  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);

  std::vector<G4int>& GetRofnumVec() { return fHitCount; }
  std::vector<G4int>& GetRofPMTIDVec() { return fPMTID; }
  std::vector<G4int>& GetTowerIEtas() { return fTowerIEta; }
  std::vector<G4int>& GetTowerIPhis() { return fTowerIPhi; }
  std::vector<G4int>& GetWavelenCount(G4int bin) { return fWavelenCount.at(bin); }
  std::vector<G4int>& GetTimeCount(G4int bin) { return fTimeCount.at(bin); }

  std::vector<G4float>& GetEdeps() { return fEdep; }
  std::vector<G4float>& GetEdepEles() { return fEdepEle; }
  std::vector<G4float>& GetEdepGammas() { return fEdepGamma; }
  std::vector<G4float>& GetEdepChargeds() { return fEdepCharged; }
  std::vector<G4int>& GetEdepTowerIEtas() { return fEdepTowerIEta; }
  std::vector<G4int>& GetEdepTowerIPhis() { return fEdepTowerIPhi; }

  std::vector<G4int>& GetRofleakIDVec_() { return fleakID; }
  std::vector<G4float>& GetRofleakVec_() { return fleakage; }
  std::vector<G4float>& GetRofleaktotalVec_() { return fleakagetotal; }

private:
  void ClearVectors();
  G4int flag;
  G4int flagnum;
  G4int flag_;
  G4int flagnum_;

  std::vector<G4int> fPMTID;
  std::vector<G4int> fHitCount;
  std::vector<G4int> fTowerIEta;
  std::vector<G4int> fTowerIPhi;
  std::vector<G4int> fSiPMCollID;
  std::vector<std::vector<G4int>> fWavelenCount;
  std::vector<std::vector<G4int>> fTimeCount;
  G4int fWavBin;
  G4int fTimeBin;

  std::vector<G4float> fEdep;
  std::vector<G4float> fEdepEle;
  std::vector<G4float> fEdepGamma;
  std::vector<G4float> fEdepCharged;
  std::vector<G4int> fEdepTowerIEta;
  std::vector<G4int> fEdepTowerIPhi;
  std::map<std::pair<int,int>, unsigned int> fEdepIterMap;

  vector<G4float> fleakage;
  vector<G4float> fleakagetotal;
  vector<G4int> fleakID;

  G4HCofThisEvent* hce;
  G4int totSDNum;
  G4int hitentries;
  DRsimPMTHitsCollection* HC;
  DRsimPMTHit* hit;
};

#endif
