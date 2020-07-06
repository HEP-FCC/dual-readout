#ifndef DRconstructor_h
#define DRconstructor_h 1

#include "DRparamBarrel.h"

#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/OpticalSurfaces.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Detector.h"

namespace ddDRcalo {
  class DRconstructor {
  public:
    DRconstructor();
    ~DRconstructor() {}

    void setXTowerDim(xml_comp_t* x_towerDim) { fX_towerDim = x_towerDim; }
    void setExpHall(dd4hep::Assembly* experimentalHall) { fExperimentalHall = experimentalHall; }
    void setDRparam(DRparamBarrel* paramBarrel) { fParamBarrel = paramBarrel; }
    void setDescription(dd4hep::Detector* description) { fDescription = description; }

    void construct();
    bool IsCerenkov(int col, int row);
    std::pair<int,int> GetColRowFromCopyNo(int copyNo, int numx);

  private:
    void implementFibers(dd4hep::Volume& tower, dd4hep::Trap& trap);

    xml_comp_t* fX_towerDim;
    dd4hep::Assembly* fExperimentalHall;
    dd4hep::Detector* fDescription;
    DRparamBarrel* fParamBarrel;
  };
}

#endif
