#include "RecoTower.h"

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
