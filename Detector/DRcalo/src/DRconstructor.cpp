#include "DRconstructor.h"

ddDRcalo::DRconstructor::DRconstructor() {
  fX_towerDim = nullptr;
  fX_sipmDim = nullptr;
  fExperimentalHall = nullptr;
  fParamBarrel = nullptr;
  fDescription = nullptr;
  fDetElement = nullptr;
  fSensDet = nullptr;
  fSipmSurf = nullptr;
  fFilterSurf = nullptr;
  fSegmentation = nullptr;
  fNumx = 0;
  fNumy = 0;
  fTowerNoLR = -999;
}

void ddDRcalo::DRconstructor::construct() {
  double currentTheta = 0.;
  int towerNo = 0;
  for (xml_coll_t x_dThetaColl(*fX_towerDim,_U(deltatheta)); x_dThetaColl; ++x_dThetaColl, ++towerNo ) {
    if (towerNo > 5) continue;

    xml_comp_t x_deltaTheta = x_dThetaColl;

    fParamBarrel->SetDeltaTheta(x_deltaTheta.deltatheta());

    double currentToC = currentTheta + x_deltaTheta.deltatheta()/2.;
    currentTheta += x_deltaTheta.deltatheta();
    fParamBarrel->SetThetaOfCenter(currentToC);
    fParamBarrel->init();
    fTowerNoLR = fParamBarrel->GetIsRHS() ? towerNo : -towerNo-1;

    dd4hep::Trap assemblyEnvelop( (fX_towerDim->height()+fParamBarrel->GetSipmHeight())/2., 0., 0., fParamBarrel->GetH1(), fParamBarrel->GetBl1(), fParamBarrel->GetTl1(), 0.,
                                  fParamBarrel->GetH2sipm(), fParamBarrel->GetBl2sipm(), fParamBarrel->GetTl2sipm(), 0. );

    dd4hep::Trap tower( fX_towerDim->height()/2., 0., 0., fParamBarrel->GetH1(), fParamBarrel->GetBl1(), fParamBarrel->GetTl1(), 0.,
                        fParamBarrel->GetH2(), fParamBarrel->GetBl2(), fParamBarrel->GetTl2(), 0. );

    dd4hep::Volume towerVol( "tower", tower, fDescription->material(fX_towerDim->materialStr()) );
    towerVol.setVisAttributes(*fDescription, fX_towerDim->visStr());

    // if (towerNo!=0 && towerNo!=1 && towerNo!=45) continue;

    implementFibers(towerVol,tower); // need to reduce time consumption!

    dd4hep::Box sipmLayer( fParamBarrel->GetBl2(), fParamBarrel->GetH2(), fParamBarrel->GetSipmHeight()/2. );
    dd4hep::Volume sipmLayerVol( "sipmLayer", sipmLayer, fDescription->material(fX_sipmDim->materialStr()) );
    sipmLayerVol.setVisAttributes(*fDescription, fX_sipmDim->visStr());

    implementSipms(sipmLayerVol);

    for (int nPhi = 0; nPhi < fX_towerDim->nphi(); nPhi++) {
      auto towerId64 = fSegmentation->setVolumeID( fTowerNoLR, nPhi );
      int towerId32 = fSegmentation->getFirst32bits(towerId64);

      // copy number of assemblyVolume is unpredictable, use dummy volume to make use of copy number of afterwards
      dd4hep::Volume assemblyEnvelopVol( "assembly", assemblyEnvelop, fDescription->material("Vacuum") );
      fExperimentalHall->placeVolume( assemblyEnvelopVol, fParamBarrel->GetAssembleTransform3D(nPhi) );

      dd4hep::PlacedVolume towerPhys = assemblyEnvelopVol.placeVolume( towerVol, towerId32, dd4hep::Position(0.,0.,-fParamBarrel->GetSipmHeight()/2.) );
      towerPhys.addPhysVolID("eta", fTowerNoLR);
      towerPhys.addPhysVolID("phi", nPhi);
      towerPhys.addPhysVolID("module", 0);

      dd4hep::PlacedVolume sipmLayerPhys = assemblyEnvelopVol.placeVolume( sipmLayerVol, towerId32, dd4hep::Position(0.,0.,fX_towerDim->height()/2.) );
      sipmLayerPhys.addPhysVolID("eta", fTowerNoLR);
      sipmLayerPhys.addPhysVolID("phi", nPhi);
      sipmLayerPhys.addPhysVolID("module", 0);
    }
  }
}

void ddDRcalo::DRconstructor::implementFibers(dd4hep::Volume& towerVol, dd4hep::Trap& trap) {
  xml_comp_t x_cladC ( fX_towerDim->child( _Unicode(cladC) ) );
  xml_comp_t x_coreC ( fX_towerDim->child( _Unicode(coreC) ) );
  xml_comp_t x_coreS ( fX_towerDim->child( _Unicode(coreS) ) );

  dd4hep::Tube fiber = dd4hep::Tube(0.,x_cladC.rmax(),fX_towerDim->height()/2.);
  dd4hep::Tube fiberC = dd4hep::Tube(0.,x_coreC.rmin(),fX_towerDim->height()/2.);
  dd4hep::Tube fiberS = dd4hep::Tube(0.,x_coreS.rmin(),fX_towerDim->height()/2.);

  float sipmSize = fX_towerDim->dx();
  float gridSize = fX_towerDim->distance();

  fNumx = static_cast<int>( std::floor( ( fParamBarrel->GetBl2()*2. - sipmSize )/gridSize ) ) + 1; // in eta direction
  fNumy = static_cast<int>( std::floor( ( fParamBarrel->GetH2()*2. - sipmSize )/gridSize ) ) + 1; // in phi direction

  fGridX.clear();
  fGridY.clear();

  for (int j = 0; j < fNumy; j++) {
    for (int k = 0; k < fNumx; k++) {
      float ptX = -gridSize*static_cast<float>(fNumx/2) + static_cast<float>(k)*gridSize + ( fNumx%2==0 ? gridSize/2. : 0. );
      float ptY = -gridSize*static_cast<float>(fNumy/2) + static_cast<float>(j)*gridSize + ( fNumy%2==0 ? gridSize/2. : 0. );
      fGridX.push_back(ptX);
      fGridY.push_back(ptY);
    }
  }

  for (unsigned int j = 0; j < fGridX.size(); j++) {
    auto colrow = GetColRowFromCopyNo(static_cast<int>(j), fNumx);
    int column = colrow.first;
    int row = colrow.second;

    dd4hep::RotationZYX rot = dd4hep::RotationZYX(M_PI, 0., 0.); // AdHoc rotation, potentially bug
    dd4hep::Position pos = dd4hep::Position(fGridX.at(j),fGridY.at(j),0.);
    dd4hep::Transform3D trans = dd4hep::Transform3D(rot,pos);

    auto fiberId64 = fSegmentation->setVolumeID(fTowerNoLR, 0, fNumx, fNumy, column, row);
    int fiberId32 = fSegmentation->getLast32bits(fiberId64);

    if ( fSegmentation->IsCerenkov(column,row) ) { //c fibre
      dd4hep::IntersectionSolid intersectClad("cladC",fiber,trap,trans);
      dd4hep::Volume cladVol("cladC", intersectClad, fDescription->material(x_cladC.materialStr()));
      cladVol.setVisAttributes(*fDescription, x_cladC.visStr());
      towerVol.placeVolume( cladVol, fiberId32, trans );

      dd4hep::IntersectionSolid intersectCore("coreC",fiberC,trap,trans);
      dd4hep::Volume coreVol("coreC", intersectCore, fDescription->material(x_coreC.materialStr()));
      coreVol.setVisAttributes(*fDescription, x_coreC.visStr());
      cladVol.placeVolume( coreVol, fiberId32 );

      // fCerenRegion->AddRootLogicalVolume(cladLogical);
      // fCerenRegion->AddRootLogicalVolume(coreLogical);
      // cladLogical->SetRegion(fCerenRegion);
      // coreLogical->SetRegion(fCerenRegion);

    } else { // s fibre
      dd4hep::IntersectionSolid intersectClad("cladS",fiber,trap,trans);
      dd4hep::Volume cladVol("cladS", intersectClad, fDescription->material(x_coreC.materialStr()));
      cladVol.setVisAttributes(*fDescription, x_coreC.visStr());
      towerVol.placeVolume( cladVol, fiberId32, trans );

      dd4hep::IntersectionSolid intersectCore("coreS",fiberS,trap,trans);
      dd4hep::Volume coreVol("coreS", intersectCore, fDescription->material(x_coreS.materialStr()));
      coreVol.setVisAttributes(*fDescription, x_coreS.visStr());
      cladVol.placeVolume( coreVol, fiberId32 );

      // fScintRegion->AddRootLogicalVolume(cladLogical);
      // fScintRegion->AddRootLogicalVolume(coreLogical);
      // cladLogical->SetRegion(fScintRegion);
      // coreLogical->SetRegion(fScintRegion);

    }
  }
}

void ddDRcalo::DRconstructor::implementSipms(dd4hep::Volume& sipmLayerVol) {
  xml_comp_t x_glass ( fX_sipmDim->child( _Unicode(sipmGlass) ) );
  xml_comp_t x_wafer ( fX_sipmDim->child( _Unicode(sipmWafer) ) );
  xml_comp_t x_filter ( fX_sipmDim->child( _Unicode(filter) ) );

  float sipmSize = fX_towerDim->dx();
  double sipmHeight = fParamBarrel->GetSipmHeight() - x_filter.height();

  // Glass box
  dd4hep::Box sipmEnvelop(sipmSize/2., sipmSize/2., sipmHeight/2.);
  dd4hep::Volume sipmEnvelopVol( "sipmEnvelop", sipmEnvelop, fDescription->material(x_glass.materialStr()) );
  sipmEnvelopVol.setVisAttributes(*fDescription, fX_sipmDim->visStr());

  // Photosensitive wafer
  dd4hep::Box sipmWaferBox( sipmSize/2., sipmSize/2., x_wafer.height()/2. );
  dd4hep::Volume sipmWaferVol( "sipmWafer", sipmWaferBox, fDescription->material(x_wafer.materialStr()) );
  sipmWaferVol.setVisAttributes(*fDescription, x_wafer.visStr());
  dd4hep::SkinSurface(*fDescription, *fDetElement, "SiPMSurf_Tower"+std::to_string(fTowerNoLR), *fSipmSurf, sipmWaferVol);
  sipmEnvelopVol.placeVolume( sipmWaferVol, dd4hep::Position(0., 0., (sipmHeight-x_wafer.height())/2.) );

  if (x_wafer.isSensitive()) {
    sipmWaferVol.setSensitiveDetector(*fSensDet);
  }

  // Kodak filter
  dd4hep::Box filterBox( sipmSize/2., sipmSize/2., x_filter.height()/2. );
  dd4hep::Volume filterVol( "filter", filterBox, fDescription->material(x_filter.materialStr()) );
  filterVol.setVisAttributes(*fDescription, x_filter.visStr());

  for (unsigned int j = 0; j < fGridX.size(); j++) {
    auto colrow = GetColRowFromCopyNo(static_cast<int>(j), fNumx);
    int column = colrow.first;
    int row = colrow.second;

    auto sipmId64 = fSegmentation->setVolumeID(fTowerNoLR, 0, fNumx, fNumy, column, row);
    int sipmId32 = fSegmentation->getLast32bits(sipmId64);

    dd4hep::RotationZYX rot = dd4hep::RotationZYX(M_PI, 0., 0.); // AdHoc rotation, potentially bug
    dd4hep::Position pos = dd4hep::Position(fGridX.at(j),fGridY.at(j),-x_filter.height()/2.);
    if ( !fSegmentation->IsCerenkov(column,row) ) pos = dd4hep::Position(fGridX.at(j),fGridY.at(j),x_filter.height()/2.);
    dd4hep::Transform3D trans = dd4hep::Transform3D(rot,pos);

    auto sipmEnvelopPlaced = sipmLayerVol.placeVolume( sipmEnvelopVol, sipmId32, trans );
    sipmEnvelopPlaced.addPhysVolID("eta",fTowerNoLR);
    sipmEnvelopPlaced.addPhysVolID("phi",0); // initialize by dummy
    sipmEnvelopPlaced.addPhysVolID("xmax",fNumx);
    sipmEnvelopPlaced.addPhysVolID("ymax",fNumy);
    sipmEnvelopPlaced.addPhysVolID("x",column);
    sipmEnvelopPlaced.addPhysVolID("y",row);
    sipmEnvelopPlaced.addPhysVolID("c",fSegmentation->IsCerenkov(column,row));
    sipmEnvelopPlaced.addPhysVolID("module",1);

    if ( !fSegmentation->IsCerenkov(column,row) ) { //s channel
      dd4hep::Position posFilter = dd4hep::Position(fGridX.at(j),fGridY.at(j),-sipmHeight/2.);
      dd4hep::Transform3D transFilter = dd4hep::Transform3D(rot,posFilter);

      dd4hep::PlacedVolume filterPlaced = sipmLayerVol.placeVolume( filterVol, sipmId32, transFilter );
      dd4hep::BorderSurface(*fDescription, *fDetElement, "FilterSurf_Tower"+std::to_string(fTowerNoLR)+"SiPM"+std::to_string(j), *fFilterSurf, filterPlaced, sipmEnvelopPlaced);
    }
  }
}

std::pair<int,int> ddDRcalo::DRconstructor::GetColRowFromCopyNo(int copyNo, int numx) {
  int column = copyNo % numx;
  int row = copyNo / numx;

  return std::make_pair(column,row);
}
