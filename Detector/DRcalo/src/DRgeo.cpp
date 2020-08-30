#include "DRparamBarrel.h"
#include "DRparamEndcap.h"
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
    xml_comp_t x_barrel ( x_det.child( _Unicode(barrel) ) );
    xml_comp_t x_endcap ( x_det.child( _Unicode(endcap) ) );
    xml_comp_t x_structure ( x_det.child( _Unicode(structure) ) );
    xml_comp_t x_dim ( x_structure.child( _Unicode(dim) ) );
    xml_comp_t x_sipmDim ( x_det.child( _Unicode(sipmDim) ) );

    dd4hep::OpticalSurfaceManager surfMgr = description.surfaceManager();
    dd4hep::OpticalSurface sipmSurfProp = surfMgr.opticalSurface("/world/"+name+"#SiPMSurf");
    dd4hep::OpticalSurface filterSurfProp = surfMgr.opticalSurface("/world/"+name+"#FilterSurf");

    auto segmentation = dynamic_cast<dd4hep::DDSegmentation::GridDRcalo*>( sensDet.readout().segmentation().segmentation() );
    segmentation->setGridSize( x_dim.distance() );

    auto paramBarrel = segmentation->paramBarrel();
    paramBarrel->SetInnerX(x_barrel.rmin());
    paramBarrel->SetTowerH(x_barrel.height());
    paramBarrel->SetNumZRot(x_barrel.nphi());
    paramBarrel->SetSipmHeight(x_sipmDim.height());

    auto paramEndcap = segmentation->paramEndcap();
    paramEndcap->SetInnerX(x_endcap.rmin());
    paramEndcap->SetTowerH(x_endcap.height());
    paramEndcap->SetNumZRot(x_endcap.nphi());
    paramEndcap->SetSipmHeight(x_sipmDim.height());

    auto constructor = DRconstructor(x_det);
    constructor.setExpHall(&experimentalHall);
    constructor.setDRparamBarrel(paramBarrel);
    constructor.setDRparamEndcap(paramEndcap);
    constructor.setDescription(&description);
    constructor.setDetElement(&drDet);
    constructor.setSipmSurf(&sipmSurfProp);
    constructor.setFilterSurf(&filterSurfProp);
    constructor.setSensDet(&sensDet);

    paramBarrel->SetIsRHS(true);
    paramEndcap->SetIsRHS(true);
    constructor.construct(); // right

    paramBarrel->SetIsRHS(false);
    paramEndcap->SetIsRHS(false);
    constructor.construct(); // left

    dd4hep::PlacedVolume hallPlace = description.pickMotherVolume(drDet).placeVolume(experimentalHall);
    hallPlace.addPhysVolID("system",x_det.id());
    // connect placed volume and physical volume
    drDet.setPlacement( hallPlace );

    paramBarrel->finalized();
    paramEndcap->finalized();

    return drDet;
  }
} // namespace detector
DECLARE_DETELEMENT(ddDRcalo, ddDRcalo::create_detector) // factory method
