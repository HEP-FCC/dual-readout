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

#endif
