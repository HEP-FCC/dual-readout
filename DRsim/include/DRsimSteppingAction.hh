#ifndef DRsimSteppingAction_h
#define DRsimSteppingAction_h 1

#include "DRsimInterface.h"
#include "DRsimEventAction.hh"

#include "G4UserSteppingAction.hh"
#include "G4LogicalVolume.hh"
#include "G4Step.hh"

using namespace std;

class DRsimSteppingAction : public G4UserSteppingAction {
public:
  DRsimSteppingAction(DRsimEventAction* eventAction);
  virtual ~DRsimSteppingAction();
  virtual void UserSteppingAction(const G4Step*);

private:
  DRsimEventAction* fEventAction;
  DRsimInterface::DRsimEdepData fEdep;
  DRsimInterface::DRsimLeakageData fLeak;
  
  G4VPhysicalVolume* GetMotherTower(G4TouchableHandle touchable) { return touchable->GetVolume(touchable->GetHistoryDepth()-1); }

  G4int GetTowerITheta(G4String towerName) {
    G4int towerITheta_ = std::stoi(towerName.substr(3));
    bool IsR = (towerName.find("R")==std::string::npos) ? false : true;
    bool IsB = (towerName.find("B")==std::string::npos) ? false : true;
    if(!IsB) towerITheta_ += 52;
    if(!IsR) towerITheta_ = -towerITheta_-1;

    return towerITheta_;
  }

  G4int GetTowerIPhi(G4VPhysicalVolume* motherTower_) {
    return motherTower_->GetCopyNo();
  }
};

#endif
