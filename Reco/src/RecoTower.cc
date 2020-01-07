#include "RecoTower.h"
#include "RecoInterface.h"

#include "Riostream.h"

RecoTower::RecoTower() {}

RecoTower::~RecoTower() {}

void RecoTower::readCSV(std::vector<std::pair<float,float>>& calibs, std::string filename) {
  std::ifstream in;
  int i;
  float ceren, scint;

  in.open(filename,std::ios::in);
  while (true) {
    in >> i >> ceren >> scint;
    if (!in.good()) break;
    calibs.push_back(std::make_pair(ceren,scint));
  }
  in.close();
}

void RecoTower::reconstruct(const DRsimInterface::DRsimTowerData& tower) {
  RecoInterface::RecoTowerData recoTower(tower);

  for (auto sipmItr = tower.SiPMs.begin(); sipmItr != tower.SiPMs.end(); ++sipmItr) {
    auto sipm = *sipmItr;
    
    for (auto timeItr = sipm.timeStruct.begin(); timeItr != sipm.timeStruct.end(); ++timeItr) {
      auto timeObj = *timeItr;

    }


  } // sipm loop

}
