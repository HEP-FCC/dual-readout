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

    long long int fiberNum;
    float E;
    float Ecorr;
    int n;
    float t;
    float depth;
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
    int iTheta;
    int iPhi;
    int numx;
    int numy;
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
};

#endif
