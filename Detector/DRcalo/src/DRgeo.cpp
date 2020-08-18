#include "DRparamBarrel.h"
#include "DRconstructor.h"
#include "GridDRcaloHandle.h"

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
    // set the sensitive detector type to the DD4hep calorimeter
    dd4hep::xml::Dimension sensDetType = xmlElement.child(_Unicode(sensitive));
    sensDet.setType(sensDetType.typeStr());
    // Get the world volume
    dd4hep::Assembly experimentalHall("hall");
    // Get the dimensions defined in the xml-tree
    xml_comp_t x_towerDim ( x_det.child( _U(trap) ) );
    xml_comp_t x_sipmDim ( x_det.child( _Unicode(sipmDim) ) );

    dd4hep::OpticalSurfaceManager surfMgr = description.surfaceManager();
    dd4hep::OpticalSurface sipmSurfProp = surfMgr.opticalSurface("/world/"+name+"#SiPMSurf");
    dd4hep::OpticalSurface filterSurfProp = surfMgr.opticalSurface("/world/"+name+"#FilterSurf");

    auto segmentation = dynamic_cast<dd4hep::DDSegmentation::GridDRcalo*>( sensDet.readout().segmentation().segmentation() );
    segmentation->setGridSize( x_towerDim.distance() );

    auto paramBarrel = new DRparamBarrel();
    paramBarrel->SetInnerX(x_towerDim.rmin());
    paramBarrel->SetTowerH(x_towerDim.height());
    paramBarrel->SetNumZRot(x_towerDim.nphi());
    paramBarrel->SetSipmHeight(x_sipmDim.height());

    auto constructor = DRconstructor();
    constructor.setXdet(&x_det);
    constructor.setXTowerDim(&x_towerDim);
    constructor.setExpHall(&experimentalHall);
    constructor.setDRparam(paramBarrel);
    constructor.setDescription(&description);
    constructor.setXSipmDim(&x_sipmDim);
    constructor.setDetElement(&drDet);
    constructor.setSipmSurf(&sipmSurfProp);
    constructor.setFilterSurf(&filterSurfProp);
    constructor.setSensDet(&sensDet);

    paramBarrel->SetIsRHS(true);
    constructor.construct(); // Barrel right
    paramBarrel->filled();

    paramBarrel->SetIsRHS(false);
    constructor.construct(); // Barrel left

    dd4hep::PlacedVolume hallPlace = description.pickMotherVolume(drDet).placeVolume(experimentalHall);
    hallPlace.addPhysVolID("system",x_det.id());
    // connect placed volume and physical volume
    drDet.setPlacement( hallPlace );

    return drDet;
  }
} // namespace detector
DECLARE_DETELEMENT(ddDRcalo, ddDRcalo::create_detector) // factory method
