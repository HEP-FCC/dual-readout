#ifndef DRsimInterface_h
#define DRsimInterface_h 1

#include <vector>
#include <utility>
#include <map>
#include <tuple>

class DRsimInterface {
public:
  DRsimInterface();
  ~DRsimInterface();

  typedef std::pair<float,float> hitRange;
  typedef std::pair<int,int> hitXY;
  typedef std::map<hitRange, int> DRsimTimeStruct;
  typedef std::map<hitRange, int> DRsimWavlenSpectrum;
  typedef std::tuple<float,float,float> threeVector;

  struct DRsimTowerProperty {
    DRsimTowerProperty() {};
    virtual ~DRsimTowerProperty() {};

    std::pair<int,float> towerTheta;
    DRsimInterface::hitXY towerXY;
    float innerR;
    float towerH;
    float dTheta;
  };

  struct DRsimSiPMData {
    DRsimSiPMData() {};
    virtual ~DRsimSiPMData() {};

    int count;
    int SiPMnum;
    int x;
    int y;
    threeVector pos;
    DRsimTimeStruct timeStruct;
    DRsimWavlenSpectrum wavlenSpectrum;
  };

  struct DRsimTowerData {
    DRsimTowerData() {};
    virtual ~DRsimTowerData() {};

    std::pair<int,float> towerTheta;
    std::pair<int,float> towerPhi;
    int numx;
    int numy;
    float innerR;
    float towerH;
    float dTheta;
    std::vector<DRsimSiPMData> SiPMs;
  };

  struct DRsimEdepData {
    DRsimEdepData() {};
    virtual ~DRsimEdepData() {};

    float Edep;
    float EdepEle;
    float EdepGamma;
    float EdepCharged;
    int iTheta;
    int iPhi;
  };

  struct DRsimLeakageData {
    DRsimLeakageData() {};
    virtual ~DRsimLeakageData() {};

    float E;
    float px;
    float py;
    float pz;
    float vx;
    float vy;
    float vz;
    float vt;
    int pdgId;
  };

  struct DRsimGenData {
    DRsimGenData() {};
    virtual ~DRsimGenData() {};

    float E;
    float px;
    float py;
    float pz;
    float vx;
    float vy;
    float vz;
    float vt;
    int pdgId;
  };

  struct DRsimEventData {
    DRsimEventData() {};
    virtual ~DRsimEventData() {};

    int event_number;
    std::vector<DRsimTowerData> towers;
    std::vector<DRsimEdepData> Edeps;
    std::vector<DRsimLeakageData> leaks;
    std::vector<DRsimGenData> GenPtcs;
  };

};

#endif
