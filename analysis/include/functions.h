#ifndef functions_h
#define functions_h 1

#include "TH1F.h"
#include <vector>

class TString;

namespace functions {
float E_DR(float E_C, float E_S);
float E_DR291(float E_C, float E_S);

class dualhist1D {
public:
  dualhist1D(TString name, TString title, int nbins, double start, double end);
  virtual ~dualhist1D() {};

  TH1F* getHist(int type) { return (type == 0) ? mHistS.get() : mHistC.get(); }

private:
  std::unique_ptr<TH1F> mHistS;
  std::unique_ptr<TH1F> mHistC;
};
} // namespace functions

#endif
