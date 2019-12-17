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

  std::map<G4String, int> towerNameMap = {
    {"Copper",0}, {"Polystyrene",2}, {"PMMA_Clad",1}, {"PMMA",2}, {"Fluorinated_Polymer",1}, {"Silicon",3}, {"Glass",2}, {"gelatin",2}, {"GlassLayer",1}
  };

  G4VPhysicalVolume* GetMotherTower(G4String matName, G4TouchableHandle touchable) {
    return touchable->GetVolume(towerNameMap[matName]);
  }

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
