#include "fastjetInterface.h"
#include <vector>

#if defined(__CLING__) || defined(__CINT__)
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ struct fastjetInterface::fastjetData+;
#pragma link C++ struct fastjetInterface::fastjetDataBase+;
#pragma link C++ class std::vector<fastjetInterface::fastjetData>+;
#pragma link C++ class std::vector<fastjetInterface::fastjetDataBase>+;

#pragma link C++ class DRsimInterface::threeVector+;
#pragma link C++ struct DRsimInterface::DRsimSiPMData+;
#pragma link C++ struct DRsimInterface::DRsimTowerData+;
#pragma link C++ struct DRsimInterface::DRsimEdepData+;
#pragma link C++ struct DRsimInterface::DRsimLeakageData+;
#pragma link C++ struct DRsimInterface::DRsimGenData+;
#pragma link C++ struct DRsimInterface::DRsimEventData+;

#pragma link C++ struct RecoInterface::RecoFiberData+;
#pragma link C++ struct RecoInterface::RecoTowerData+;
#pragma link C++ struct RecoInterface::RecoEventData+;

#endif
