#include "DRparamBarrel.h"
#include "DRconstructor.h"

#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/OpticalSurfaces.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Detector.h"

namespace ddDRcalo {
  static dd4hep::Ref_t create_detector( dd4hep::Detector &description, xml_h xmlElement, dd4hep::SensitiveDetector sensDet ) {
    // Get the detector description from the xml-tree
    xml_det_t    x_det = xmlElement;
    std::string  name  = x_det.nameStr();
    // Create the detector element
    dd4hep::DetElement drDet( name, x_det.id() );
    // Get the world volume
    dd4hep::Assembly experimentalHall("hall");
    // Get the dimensions defined in the xml-tree
    xml_comp_t x_towerDim ( x_det.child( _U(trap) ) );

    auto paramBarrel = DRparamBarrel();
    double currentTheta = 0.;
    paramBarrel.SetInnerX(x_towerDim.rmin());
    paramBarrel.SetTowerH(x_towerDim.height());
    paramBarrel.SetNumZRot(x_towerDim.nphi());
    paramBarrel.SetPMTT(0.);

    auto constructor = DRconstructor();
    constructor.setXTowerDim(&x_towerDim);
    constructor.setExpHall(&experimentalHall);
    constructor.setDRparam(&paramBarrel);
    constructor.setDescription(&description);

    paramBarrel.SetIsRHS(true);
    constructor.construct(); // Barrel right

    paramBarrel.SetIsRHS(false);
    constructor.construct(); // Barrel left

    dd4hep::PlacedVolume hallPlace = description.pickMotherVolume(drDet).placeVolume(experimentalHall);
    hallPlace.addPhysVolID("system",x_det.id());
    // connect placed volume and physical volume
    drDet.setPlacement( hallPlace );

    return drDet;
  }
} // namespace detector
DECLARE_DETELEMENT(ddDRcalo, ddDRcalo::create_detector) // factory method
