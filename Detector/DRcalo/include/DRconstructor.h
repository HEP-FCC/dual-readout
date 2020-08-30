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
    DRconstructor(xml_det_t& x_det);
    ~DRconstructor() {}

    void setExpHall(dd4hep::Assembly* experimentalHall) { fExperimentalHall = experimentalHall; }
    void setDRparamBarrel(dd4hep::DDSegmentation::DRparamBarrel* paramBarrel) { fParamBarrel = paramBarrel; }
    void setDRparamEndcap(dd4hep::DDSegmentation::DRparamEndcap* paramEndcap) { fParamEndcap = paramEndcap; }
    void setDescription(dd4hep::Detector* description) { fDescription = description; }
    void setDetElement(dd4hep::DetElement* drDet) { fDetElement = drDet; }
    void setSipmSurf(dd4hep::OpticalSurface* sipmSurf) { fSipmSurf = sipmSurf; }
    void setFilterSurf(dd4hep::OpticalSurface* filterSurf) { fFilterSurf = filterSurf; }
    void setSensDet(dd4hep::SensitiveDetector* sensDet) {
      fSensDet = sensDet;
      fSegmentation = dynamic_cast<dd4hep::DDSegmentation::GridDRcalo*>( sensDet->readout().segmentation().segmentation() );
    }

    void construct();

  private:
    void implementTowers(xml_comp_t& x_theta, dd4hep::DDSegmentation::DRparamBase* param);
    void implementFibers(xml_comp_t& x_theta, dd4hep::Volume& towerVol, dd4hep::Trap& trap, dd4hep::DDSegmentation::DRparamBase* param);
    void implementFiber(dd4hep::Volume& towerVol, dd4hep::Position& pos, int col, int row,
                        dd4hep::Tube& fiber, dd4hep::Tube& fiberC, dd4hep::Tube& fiberS);
    void implementSipms(dd4hep::Volume& sipmLayerVol);
    double calculateDistAtZ(TGeoTrap* rootTrap, dd4hep::Position& pos, double* norm, double z);
    float calculateFiberLen(TGeoTrap* rootTrap, dd4hep::Position& pos, double* norm, double z1, double diff, double towerHeight);
    void checkContained(TGeoTrap* rootTrap, dd4hep::Position& pos, double z);
    void getNormals(TGeoTrap* rootTrap, double z, double* norm1, double* norm2, double* norm3, double* norm4);

    xml_det_t fX_det;
    xml_comp_t fX_barrel;
    xml_comp_t fX_endcap;
    xml_comp_t fX_sipmDim;
    xml_comp_t fX_struct;
    xml_comp_t fX_dim;
    xml_comp_t fX_cladC;
    xml_comp_t fX_coreC;
    xml_comp_t fX_coreS;
    dd4hep::Assembly* fExperimentalHall;
    dd4hep::Detector* fDescription;
    dd4hep::DDSegmentation::DRparamBarrel* fParamBarrel;
    dd4hep::DDSegmentation::DRparamEndcap* fParamEndcap;
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
