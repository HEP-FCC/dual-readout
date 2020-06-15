#ifndef DRsimSteppingAction_h
#define DRsimSteppingAction_h 1

#include "DRsimInterface.h"
#include "DRsimEventAction.hh"

#include "G4UserSteppingAction.hh"
#include "G4LogicalVolume.hh"
#include "G4Step.hh"
#include "G4SteppingManager.hh"

using namespace std;

class DRsimSteppingAction : public G4UserSteppingAction {
public:
  DRsimSteppingAction(DRsimEventAction* eventAction);
  virtual ~DRsimSteppingAction();
  virtual void UserSteppingAction(const G4Step*);

private:
  DRsimEventAction* fEventAction;
  DRsimInterface::DRsimLeakageData fLeak;
  bool fIsR;
  bool fIsB;

  G4VPhysicalVolume* GetMotherTower(G4TouchableHandle touchable) { return touchable->GetVolume(touchable->GetHistoryDepth()-1); }
  G4int GetTowerIPhi(G4VPhysicalVolume* motherTower_) { return motherTower_->GetCopyNo(); }

  G4int GetTowerITheta(G4String towerName) {
    G4int towerITheta_ = std::stoi(towerName.substr(3));
    fIsR = (towerName.find("R")==std::string::npos) ? false : true;
    fIsB = (towerName.find("B")==std::string::npos) ? false : true;
    if(!fIsB) towerITheta_ += 52;
    if(!fIsR) towerITheta_ = -towerITheta_-1;

    return towerITheta_;
  }
};

#endif
