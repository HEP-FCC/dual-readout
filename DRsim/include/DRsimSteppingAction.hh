#ifndef DRsimSteppingAction_h
#define DRsimSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include <stdio.h>
#include "G4LogicalVolume.hh"
#include "G4Step.hh"
#include "G4TrackVector.hh"
#include <map>

using namespace std;
class DRsimEventAction;
class G4LogicalVolume;

class DRsimSteppingAction : public G4UserSteppingAction {
public:
  DRsimSteppingAction(DRsimEventAction* eventAction);
  virtual ~DRsimSteppingAction();
  virtual void UserSteppingAction(const G4Step*);

private:
  DRsimEventAction* fEventAction;
  G4LogicalVolume* fScoringVolume;

  bool postvolnull;
  G4int fullID;
  G4double pdgCharge;
  G4int pdgID;

  G4LogicalVolume* pre_volume;
  G4StepPoint* presteppoint;
  G4StepPoint* poststeppoint;
  G4TouchableHandle theTouchable;
  G4ParticleDefinition* particle;
  G4Track* track;
  G4float fEdep;
  G4float fEdepEle;
  G4float fEdepGamma;
  G4float fEdepCharged;
  G4VPhysicalVolume* motherTower;
  G4int towerIEta;
  G4int towerIPhi;
  G4double sl;

  std::map<G4String, int> towerNameMap = {
    {"Copper",0}, {"Polystyrene",2}, {"PMMA_Clad",1}, {"PMMA",2}, {"Fluorinated_Polymer",1}, {"Silicon",3}, {"Glass",2}, {"gelatin",2}, {"GlassLayer",1}
  };

  G4VPhysicalVolume* GetMotherTower(G4String matName, G4TouchableHandle touchable) {
    return touchable->GetVolume(towerNameMap[matName]);
  }

  G4int GetTowerIEta(G4String towerName) {
    G4int towerIEta_ = std::stoi(towerName.substr(3));
    bool IsR = (towerName.find("R")==std::string::npos) ? false : true;
    bool IsB = (towerName.find("B")==std::string::npos) ? false : true;
    if(!IsB) towerIEta_ += 52;
    if(!IsR) towerIEta_ = -towerIEta_ -1;

    return towerIEta_;
  }

  G4int GetTowerIPhi(G4VPhysicalVolume* motherTower_) {
    return motherTower_->GetCopyNo();
  }
};

#endif
