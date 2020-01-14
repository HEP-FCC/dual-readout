#ifndef RecoInterface_h
#define RecoInterface_h 1

#include "DRsimInterface.h"

class RecoInterface {
public:
  RecoInterface() {};
  ~RecoInterface() {};

  struct RecoFiberData {
    RecoFiberData() {};
    RecoFiberData(const DRsimInterface::DRsimSiPMData& sipmIn);
    ~RecoFiberData() {};

    bool IsCerenkov;
    float E;
    float Ecorr;
    int n;
    float t;
    int x;
    int y;
    float depth;
    DRsimInterface::threeVector pos;
  };

  struct RecoTowerData {
    RecoTowerData() {};
    RecoTowerData(const DRsimInterface::DRsimTowerData& towerIn);
    ~RecoTowerData() {};

    float E_C;
    float E_S;
    float E_Scorr;
    float E_DR;
    float E_DRcorr;
    int n_C;
    int n_S;
    std::pair<int,float> theta;
    std::pair<int,float> phi;
    int numx;
    int numy;
    float innerR;
    float towerH;
    float dTheta;
    std::vector<RecoFiberData> fibers;
  };

  struct RecoEventData {
    RecoEventData();
    ~RecoEventData() {};

    float E_C;
    float E_S;
    float E_Scorr;
    float E_DR;
    float E_DRcorr;
    int n_C;
    int n_S;
    std::vector<RecoTowerData> towers;
  };

  static bool IsCerenkov(int col, int row);
};

#endif
