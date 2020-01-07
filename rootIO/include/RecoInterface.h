#ifndef RecoInterface_h
#define RecoInterface_h 1

#include "DRsimInterface.h"

class RecoInterface {
public:
  RecoInterface() {};
  ~RecoInterface() {};

  struct RecoFiberData {
    RecoFiberData() {};
    ~RecoFiberData() {};

    bool IsCerenkov;
    float E;
    float Ecorr;
    int n;
    float t;
    int x;
    int y;
    float theta;
    float phi;
    float depth;
  };

  struct RecoTowerData {
    RecoTowerData() {};
    RecoTowerData(const DRsimInterface::DRsimTowerData& towerIn);
    ~RecoTowerData() {};

    float E_C;
    float E_S;
    float E_Scorr;
    int n_C;
    int n_S;
    float t_C;
    float t_S;
    std::pair<int,float> theta;
    std::pair<int,float> phi;
    int numx;
    int numy;
    float innerR;
    float towerH;
    float dTheta;
    float depth;
    std::vector<RecoFiberData> Fibers;
  };
};

#endif
