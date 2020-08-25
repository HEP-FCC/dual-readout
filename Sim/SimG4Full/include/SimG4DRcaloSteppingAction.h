#ifndef SimG4DRcaloSteppingAction_h
#define SimG4DRcaloSteppingAction_h 1

#include "DRsimInterface.h"

#include "GeoSvc.h"
#include "GridDRcalo.h"

#include "G4UserSteppingAction.hh"

class SimG4DRcaloSteppingAction : public G4UserSteppingAction {
public:
  SimG4DRcaloSteppingAction();
  virtual ~SimG4DRcaloSteppingAction();

  virtual void UserSteppingAction(const G4Step*);

  void initialize();

  void setEventData(DRsimInterface::DRsimEventData* evtData) { fEventData = evtData; }

private:
  template <typename T>
  void accumulate(std::vector<T> &input, unsigned int &prev, long long int fiberId64,
                  float edep, float edepEle, float edepGamma, float edepCharged);
  bool checkId(DRsimInterface::DRsimEdepData edep, long long int id64);
  bool checkId(DRsimInterface::DRsimEdepFiberData edep, long long int id64);
  DRsimInterface::DRsimEdepData create(long long int id64, std::vector<DRsimInterface::DRsimEdepData> &dummy);
  DRsimInterface::DRsimEdepFiberData create(long long int id64, std::vector<DRsimInterface::DRsimEdepFiberData> &dummy);
  
  /// Pointer to the geometry service
  GeoSvc* m_geoSvc;

  DRsimInterface::DRsimEventData* fEventData;
  dd4hep::DDSegmentation::GridDRcalo* fSeg;

  /// Name of the readout to save
  std::string m_readoutName;

  unsigned int fPrevTower;
  unsigned int fPrevFiber;
};

#endif
