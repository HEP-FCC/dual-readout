#ifndef RecoTower_h
#define RecoTower_h 1

#include <utility>
#include <vector>
#include <iostream>

class RecoTower {
public:
  RecoTower();
  ~RecoTower();

  void readCSV(std::vector<std::pair<float,float>>& calibs, std::string filename="calib.csv");

};

#endif
