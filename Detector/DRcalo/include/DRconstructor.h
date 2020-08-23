#ifndef DRconstructor_h
#define DRconstructor_h 1

#include "DRparamBarrel.h"
#include "GridDRcaloHandle.h"

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
    void setDRparam(dd4hep::DDSegmentation::DRparamBarrel* paramBarrel) { fParamBarrel = paramBarrel; }
    void setDescription(dd4hep::Detector* description) { fDescription = description; }
    void setXSipmDim(xml_comp_t* x_sipmDim) { fX_sipmDim = x_sipmDim; }
    void setDetElement(dd4hep::DetElement* drDet) { fDetElement = drDet; }
    void setSipmSurf(dd4hep::OpticalSurface* sipmSurf) { fSipmSurf = sipmSurf; }
    void setFilterSurf(dd4hep::OpticalSurface* filterSurf) { fFilterSurf = filterSurf; }
    void setSensDet(dd4hep::SensitiveDetector* sensDet) {
      fSensDet = sensDet;
      fSegmentation = dynamic_cast<dd4hep::DDSegmentation::GridDRcalo*>( sensDet->readout().segmentation().segmentation() );
    }
    void setXdet(xml_det_t* x_det) { fX_det = x_det; }

    void construct();

  private:
    void implementFibers(dd4hep::Volume& towerVol, dd4hep::Trap& trap);
    void implementFiber(dd4hep::Volume& towerVol, dd4hep::Position& pos, int col, int row,
                        dd4hep::Tube& fiber, dd4hep::Tube& fiberC, dd4hep::Tube& fiberS);
    void implementSipms(dd4hep::Volume& sipmLayerVol);
    double calculateDistAtZ(TGeoTrap* rootTrap, dd4hep::Position& pos, double z);

    xml_det_t* fX_det;
    xml_comp_t* fX_towerDim;
    xml_comp_t* fX_sipmDim;
    dd4hep::Assembly* fExperimentalHall;
    dd4hep::Detector* fDescription;
    dd4hep::DDSegmentation::DRparamBarrel* fParamBarrel;
    dd4hep::DetElement* fDetElement;
    dd4hep::SensitiveDetector* fSensDet;
    dd4hep::OpticalSurface* fSipmSurf;
    dd4hep::OpticalSurface* fFilterSurf;
    dd4hep::DDSegmentation::GridDRcalo* fSegmentation;

    bool fVis;
    int fNumx, fNumy;
    int fTowerNoLR;
  };
}

#endif
