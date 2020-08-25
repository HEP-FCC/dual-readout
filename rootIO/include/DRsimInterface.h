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
  typedef std::map<hitRange, int> DRsimTimeStruct;
  typedef std::map<hitRange, int> DRsimWavlenSpectrum;

  struct DRsimSiPMData {
    DRsimSiPMData() {};
    virtual ~DRsimSiPMData() {};

    int count;
    long long int SiPMnum;
    DRsimTimeStruct timeStruct;
    DRsimWavlenSpectrum wavlenSpectrum;
  };

  struct DRsimTowerData {
    DRsimTowerData() {};
    virtual ~DRsimTowerData() {};

    int iTheta;
    int iPhi;
    int numx;
    int numy;
    std::vector<DRsimSiPMData> SiPMs;
  };

  struct DRsimEdepFiberData {
    DRsimEdepFiberData();
    DRsimEdepFiberData(long long int fiberId64);
    DRsimEdepFiberData(long long int fiberId64, float edep, float edepEle, float edepGamma, float edepCharged);
    virtual ~DRsimEdepFiberData() {};

    void accumulate(float edep, float edepEle, float edepGamma, float edepCharged);

    long long int fiberNum;
    float Edep;
    float EdepEle;
    float EdepGamma;
    float EdepCharged;
  };

  struct DRsimEdepData {
    DRsimEdepData();
    DRsimEdepData(int theta, int phi);
    DRsimEdepData(int theta, int phi, float edep, float edepEle, float edepGamma, float edepCharged);
    virtual ~DRsimEdepData() {};

    void accumulate(float edep, float edepEle, float edepGamma, float edepCharged);

    float Edep;
    float EdepEle;
    float EdepGamma;
    float EdepCharged;
    int iTheta;
    int iPhi;
    std::vector<DRsimEdepFiberData> fibers;
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

    void clear();

    int event_number;
    std::vector<DRsimTowerData> towers;
    std::vector<DRsimEdepData> Edeps;
    std::vector<DRsimLeakageData> leaks;
    std::vector<DRsimGenData> GenPtcs;
  };
};

#endif
